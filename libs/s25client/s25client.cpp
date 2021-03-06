// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "Debug.h"
#include "GameManager.h"
#include "QuickStartGame.h"
#include "RTTR_AssertError.h"
#include "RTTR_Version.h"
#include "RttrConfig.h"
#include "Settings.h"
#include "SignalHandler.h"
#include "WindowManager.h"
#include "drivers/AudioDriverWrapper.h"
#include "drivers/VideoDriverWrapper.h"
#include "files.h"
#include "mygettext/mygettext.h"
#include "ogl/glAllocator.h"
#include "libsiedler2/libsiedler2.h"
#include "s25util/LocaleHelper.h"
#include "s25util/Log.h"
#include "s25util/StringConversion.h"
#include "s25util/System.h"
#include "s25util/error.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/args.hpp>
#include <boost/nowide/iostream.hpp>
#include <boost/program_options.hpp>
#include <array>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <limits>
//#include <vld.h>

#ifdef _WIN32
#include <boost/nowide/convert.hpp>
#include <windows.h>
#include <s25clientResources.h>
#if defined _DEBUG && defined _MSC_VER && defined RTTR_HWETRANS
#include <eh.h>
#endif
#endif
#ifndef _MSC_VER
#include <csignal>
#endif

namespace bfs = boost::filesystem;
namespace bnw = boost::nowide;
namespace po = boost::program_options;

/// Calls a setGlobalInstance function for the (previously) singleton type T
/// on construction and destruction
template<class T>
class SetGlobalInstanceWrapper : public T
{
    using Setter = void (*)(T*);
    Setter setter_;

public:
    template<typename... Args>
    SetGlobalInstanceWrapper(Setter setter, Args&&... args) : T(std::forward<Args>(args)...), setter_(setter)
    {
        setter_(static_cast<T*>(this));
    }
    ~SetGlobalInstanceWrapper() noexcept { setter_(nullptr); }
};

// Throw this to terminate gracefully
struct RttrExitException : std::exception
{
    int code;
    RttrExitException(int code) : code(code) {}
};

namespace {
void WaitForEnter()
{
    static bool waited = false;
    if(waited)
        return;
    waited = true;
    bnw::cout << "\n\nPress ENTER to close this window . . ." << std::endl;
    bnw::cin.clear();
    bnw::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    bnw::cin.get();
}

std::string GetProgramDescription()
{
    std::stringstream s;
    s << RTTR_Version::GetTitle() << " v" << RTTR_Version::GetVersionDate() << "-" << RTTR_Version::GetRevision() << "\n"
      << "Compiled with " << System::getCompilerName() << " for " << System::getOSName();
    return s.str();
}

    ///////////////////////////////////////////////////////////////////////////////

#if defined _WIN32 && defined _DEBUG && defined _MSC_VER && defined RTTR_HWETRANS
/**
 *  Exception-Handler, wird bei einer C-Exception ausgeführt, falls dies mit RTTR_HWETRANS und
 *  im Projekt mit den Compilerflags (/EHa) aktiviert ist.
 *
 *  @param[in] exception_type    Typ der Exception (siehe GetExceptionCode)
 *  @param[in] exception_pointer Genaue Beschreibung der Exception (siehe GetExceptionInformation)
 */
void CExceptionHandler(unsigned exception_type, _EXCEPTION_POINTERS* exception_pointer)
{
    fatal_error("C-Exception caught\n");
}
#endif // _WIN32 && _DEBUG && RTTR_HWETRANS

bool askForDebugData()
{
#ifdef _WIN32
    std::wstring title = boost::nowide::widen(_("Error"));
    std::wstring text = boost::nowide::widen(_(
      "RttR crashed. Would you like to send debug information to RttR to help us avoiding this crash in the future? Thank you very much!"));
    return (MessageBoxW(nullptr, text.c_str(), title.c_str(), MB_YESNO | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND) == IDYES);
#else
    return false;
#endif
}
bool shouldSendDebugData()
{
    return (SETTINGS.global.submit_debug_data == 1) || askForDebugData();
}

void showCrashMessage()
{
    std::string text = _("RttR crashed. Please restart the application!");
#ifdef _WIN32
    MessageBoxW(nullptr, boost::nowide::widen(text).c_str(), boost::nowide::widen(_("Error")).c_str(),
                MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND);
#else
    bnw::cerr << text << std::endl;
#endif
}

[[noreturn]] void terminateProgramm()
{
#ifdef _DEBUG
    abort();
#else
    throw RttrExitException(1);
#endif
}

[[noreturn]] void handleException(void* pCtx = nullptr)
{
    std::vector<void*> stacktrace = DebugInfo::GetStackTrace(pCtx);
    try
    {
        LogTarget target = (LOG.getFileWriter()) ? LogTarget::FileAndStderr : LogTarget::Stderr;
        LOG.write("RttR crashed. Backtrace:\n", target);
        // Don't let locale mess up addresses
        s25util::ClassicImbuedStream<std::stringstream> ss;
        for(void* p : stacktrace)
            ss << p << "\n";
        LOG.write("%1%", target) % ss.str();
    } catch(...)
    { //-V565
      // Could not write stacktrace. Ignore errors
    }
    if(shouldSendDebugData())
    {
        DebugInfo di;

        di.SendReplay();
        di.SendStackTrace(stacktrace);
    }

    if(SETTINGS.global.submit_debug_data == 0)
        showCrashMessage();

    terminateProgramm();
}

#ifdef _MSC_VER
LONG WINAPI ExceptionHandler(LPEXCEPTION_POINTERS info)
{
    handleException(info->ContextRecord);
    return EXCEPTION_EXECUTE_HANDLER;
}
#else
[[noreturn]] void ExceptionHandler(int /*sig*/)
{
    handleException();
}
#endif

void InstallSignalHandlers()
{
#ifdef _WIN32
    SetConsoleCtrlHandler(ConsoleSignalHandler, TRUE);
#else
    struct sigaction sa;
    sa.sa_handler = ConsoleSignalHandler;
    sa.sa_flags = 0; // SA_RESTART would not allow to interrupt connect call;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGPIPE, &sa, nullptr);
    sigaction(SIGALRM, &sa, nullptr);
#endif

#ifdef _MSC_VER
    SetUnhandledExceptionFilter(ExceptionHandler);
#ifdef _DEBUG
#ifdef RTTR_HWETRANS
    _set_se_translator(CExceptionHandler);
#endif // RTTR_HWETRANS
#ifdef RTTR_CRTDBG
    // Enable Memory-Leak-Detection
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF /*| _CRTDBG_CHECK_EVERY_1024_DF*/);
#endif //  RTTR_CRTDBG
#endif // _DEBUG

#else
    signal(SIGSEGV, ExceptionHandler);
#endif // _MSC_VER
}

void UninstallSignalHandlers()
{
#ifdef _WIN32
    SetConsoleCtrlHandler(ConsoleSignalHandler, FALSE);
#else
    struct sigaction sa;
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0; // SA_RESTART would not allow to interrupt connect call;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGPIPE, &sa, nullptr);
    sigaction(SIGALRM, &sa, nullptr);
#endif // _WIN32

#ifdef _MSC_VER
    SetUnhandledExceptionFilter(nullptr);
#else
    signal(SIGSEGV, SIG_DFL);
#endif
}

/**
 *  Exit-Handler, wird bei @p exit ausgeführt.
 */
void ExitHandler()
{
    Socket::Shutdown();
    UninstallSignalHandlers();

#ifdef _DEBUG
    WaitForEnter();
#endif
}

void SetAppSymbol()
{
#ifdef _WIN32
    // set console window icon
    SendMessage(GetConsoleWindow(), WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_SYMBOL)));
    SendMessage(GetConsoleWindow(), WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDI_SYMBOL)));
#endif // _WIN32
}

bool InitDirectories()
{
    // Note: Do not use logger yet. Filepath may not exist
    const std::string curPath = bfs::current_path().string();
    LOG.write("Starting in %s\n", LogTarget::Stdout) % curPath;

    // diverse dirs anlegen
    std::array<unsigned, 9> dirs = {{94, 41, 47, 48, 51, 85, 98, 99, 100}}; // settingsdir muss zuerst angelegt werden (94)

    std::string oldSettingsDir;

#ifdef _WIN32
    oldSettingsDir = RTTRCONFIG.ExpandPath("~/Siedler II.5 RttR");
#elif defined(__APPLE__)
    oldSettingsDir = RTTRCONFIG.ExpandPath("~/.s25rttr");
#endif
    if(!oldSettingsDir.empty() && bfs::is_directory(oldSettingsDir))
    {
        const std::string newSettingsDir = RTTRCONFIG.ExpandPath(FILE_PATHS[94]);
        if(bfs::exists(newSettingsDir))
        {
            s25util::error(std::string("Old and new settings directory found. Please delete the one you don't want to keep!\nOld: ")
                           + oldSettingsDir + "\nNew: " + newSettingsDir);
            return false;
        }
        boost::system::error_code ec;
        bfs::rename(oldSettingsDir, newSettingsDir, ec);
        if(ec != boost::system::errc::success)
        {
            s25util::error(std::string("Old settings directory found at ") + oldSettingsDir
                           + "\n Renaming to new name failed: " + newSettingsDir + "\nError: " + ec.message()
                           + "\nRename it yourself and/or make sure the directory is writable!");
            return false;
        }
    }

    for(unsigned dirIdx : dirs)
    {
        std::string dir = RTTRCONFIG.ExpandPath(FILE_PATHS[dirIdx]);
        boost::system::error_code ec;
        bfs::create_directories(dir, ec);
        if(ec != boost::system::errc::success)
        {
            // This writes to the log. If the log folder or file could not be created, an exception is thrown
            // Make sure we catch that
            try
            {
                s25util::error(std::string("Directory ") + dir + " could not be created.");
                s25util::error("Failed to start the game");
            } catch(const std::runtime_error& error)
            {
                LOG.write("Additional error: %1%\n", LogTarget::Stderr) % error.what();
            }
            return false;
        }
    }
    // Write this to file too, after folders are created
    LOG.setLogFilepath(RTTRCONFIG.ExpandPath(FILE_PATHS[47]));
    try
    {
        LOG.open();
        LOG.write("%1%\n\n", LogTarget::File) % GetProgramDescription();
        LOG.write("Starting in %s\n", LogTarget::File) % curPath;
    } catch(const std::exception& e)
    {
        LOG.write("Error initializing log: %1%\nSystem reports: %2%\n", LogTarget::Stderr) % e.what() % LOG.getLastError();
        return false;
    }
    return true;
}

bool InitGame(GameManager& gameManager)
{
    libsiedler2::setAllocator(new GlAllocator());

    // Socketzeug initialisieren
    if(!Socket::Initialize())
    {
        s25util::error("Could not init sockets!");
        s25util::error("Failed to start the game");
        return false;
    }

    // Spiel starten
    if(!gameManager.Start())
    {
        s25util::error("Failed to start the game");
        return false;
    }
    return true;
}

int RunProgram(po::variables_map& options)
{
    LOG.write("%1%\n\n", LogTarget::Stdout) % GetProgramDescription();
    if(!LocaleHelper::init())
        return 1;
    if(!RTTRCONFIG.Init())
        return 1;
    SetAppSymbol();
    InstallSignalHandlers();
    // Exit-Handler initialisieren
    atexit(&ExitHandler);
    if(!InitDirectories())
        return 1;

    // Zufallsgenerator initialisieren (Achtung: nur für Animations-Offsets interessant, für alles andere (spielentscheidende) wird unser
    // Generator verwendet)
    srand(static_cast<unsigned>(std::time(nullptr)));

    SetGlobalInstanceWrapper<GameManager> gameManager(setGlobalGameManager, LOG, SETTINGS, VIDEODRIVER, AUDIODRIVER, WINDOWMANAGER);
    try
    {
        if(!InitGame(gameManager))
            return 2;

        if(options.count("map") && !QuickStartGame(options["map"].as<std::string>()))
            return 1;

        // Hauptschleife
        while(gameManager.Run())
        {
#ifndef _WIN32
            killme = false;
#endif // !_WIN32
        }

        // Spiel beenden
        gameManager.Stop();
        libsiedler2::setAllocator(nullptr);
    } catch(RTTR_AssertError& error)
    {
        // Write to log file, but don't throw any errors if this fails too
        try
        {
            LOG.writeToFile(error.what());
        } catch(...)
        { //-V565
        }
        return 42;
    }
    return 0;
}
} // namespace

/**
 *  Hauptfunktion von Siedler II.5 Return to the Roots
 *
 *  @param[in] argc Anzahl übergebener Argumente
 *  @param[in] argv Array der übergebenen Argumente
 *
 *  @return Exit Status, 0 bei Erfolg, > 0 bei Fehler
 */
// Exceptions handled by registred global handlers
// NOLINTNEXTLINE(bugprone-exception-escape)
int main(int argc, char** argv)
{
    bnw::args _(argc, argv);

    po::options_description desc("Allowed options");
    // clang-format off
    desc.add_options()
        ("help,h", "Show help")
        ("map,m", po::value<std::string>(),"Map to load")
        ("version", "Show version information and exit")
        ;
    // clang-format on
    po::positional_options_description positionalOptions;
    positionalOptions.add("map", 1);

    po::variables_map options;
    try
    {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(positionalOptions).run(), options);
    } catch(const po::error& e)
    {
        bnw::cerr << "Error: " << e.what() << "\n\n";
        bnw::cerr << desc << "\n";
        return 1;
    }
    po::notify(options);

    if(options.count("help"))
    {
        bnw::cout << desc << "\n";
        return 0;
    }
    if(options.count("version"))
    {
        bnw::cout << GetProgramDescription() << std::endl;
        return 0;
    }

    int result;
    try
    {
        result = RunProgram(options);
    } catch(RttrExitException& e)
    {
        result = e.code;
    }
    if(result)
        WaitForEnter();

    return result;
}
