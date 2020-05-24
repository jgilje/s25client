// Copyright (c) 2005 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "dskCredits.h"
#include "Loader.h"
#include "WindowManager.h"
#include "controls/ctrlButton.h"
#include "drivers/VideoDriverWrapper.h"
#include "dskMainMenu.h"
#include "lua/GameDataLoader.h"
#include "ogl/FontStyle.h"
#include "ogl/MusicItem.h"
#include "ogl/glArchivItem_Bob.h"
#include "ogl/glFont.h"
#include "gameData/JobConsts.h"
#include "gameData/NationConsts.h"
#include "gameData/WorldDescription.h"
#include <array>
#include <cstdlib>

/** @class dskCredits
 *
 *  Klasse des Credits Desktops.
 */

/// Duration for one credits page
const unsigned PAGE_TIME = 12900;
/// Duration for fading between pages
const unsigned FADING_TIME = 2000;

dskCredits::dskCredits() : Desktop(LOADER.GetImageN("setup013", 0))
{
    // Zurück
    AddTextButton(0, DrawPoint(300, 550), Extent(200, 22), TC_RED1, _("Back"), NormalFont);

    AddText(1, DrawPoint(400, 10), _("Return To The Roots"), COLOR_YELLOW, FontStyle::CENTER, LargeFont);

    // "Credits"
    AddText(2, DrawPoint(400, 33), _("Credits"), COLOR_YELLOW, FontStyle::CENTER, LargeFont);

    // order by last name (alphabetical)
    CreditsEntry entry = CreditsEntry("Alexander Grund (Flamefire):", GetCreditsImgOrDefault("flamefire"));
    entry.lines.push_back(_("Programming"));
    entry.lines.push_back(_("Quality Assurance"));
    entries.push_back(entry);

    entry = CreditsEntry("Patrick Haak (Demophobie):", GetCreditsImgOrDefault("demophobie"));
    entry.lines.push_back(_("Website Administration"));
    entry.lines.push_back(_("Quality Assurance"));
    entries.push_back(entry);

    entry = CreditsEntry("Jan-Henrik Kluth (jh):", GetCreditsImgOrDefault("jh"));
    entry.lines.push_back(_("Programming"));
    entry.lines.push_back(_("Artificial Intelligence (AI)"));
    entries.push_back(entry);

    entry = CreditsEntry("Christopher Kuehnel (Spikeone):", GetCreditsImgOrDefault("spikeone"));
    entry.lines.push_back(_("Additional graphics"));
    entry.lines.push_back(_("Quality Assurance"));
    entry.lines.push_back(_("Mapping"));
    entries.push_back(entry);

    entry = CreditsEntry("Stefan Schüchl (Z-Stef):", GetCreditsImgOrDefault("z-stef"));
    entry.lines.push_back(_("Website Administration"));
    entry.lines.push_back(_("Website Programming"));
    entries.push_back(entry);

    entry = CreditsEntry("Marcus Ströbel (Maqs):", GetCreditsImgOrDefault("maqs"));
    entry.lines.push_back(_("Programming"));
    entry.lines.push_back(_("Quality Assurance"));
    entries.push_back(entry);

    entry = CreditsEntry("Jonas Trampe (NastX):", GetCreditsImgOrDefault("nastx"));
    entry.lines.push_back(_("Quality Assurance"));
    entry.lines.push_back(_("Mapping"));
    entries.push_back(entry);

    // founder members
    entry = CreditsEntry("Oliver Siebert (Oliverr):", GetCreditsImgOrDefault("oliverr"));
    entry.lines.push_back(_("Project founder"));
    entry.lines.push_back(_("Project management"));
    entry.lines.push_back(_("Programming"));
    entry.lines.push_back(_("Quality Assurance"));
    entries.push_back(entry);

    entry = CreditsEntry("Florian Doersch (FloSoft):", GetCreditsImgOrDefault("flosoft"));
    entry.lines.push_back(_("Project founder"));
    entry.lines.push_back(_("Project management"));
    entry.lines.push_back(_("Server management"));
    entry.lines.push_back(_("Programming"));
    entry.lines.push_back(_("Website Administration"));
    entry.lines.push_back(_("Website Programming"));
    entry.lines.push_back(_("Quality Assurance"));
    entry.lines.push_back(_("Finances"));
    entries.push_back(entry);

    // alphabetical again
    entry = CreditsEntry(_("Additional Programming:"));
    entry.lines.push_back("Ikhar Beq (PoC)");
    entry.lines.push_back("Cat666");
    entry.lines.push_back("Devil");
    entry.lines.push_back("Divan");
    entry.lines.push_back("Christoph Erhardt (Airhardt)");
    entry.lines.push_back("Siegfried Oleg Pammer (siegi44)");
    entry.lines.push_back("Lienhart Woitok (liwo)");
    entry.lines.push_back("");
    entry.lines.push_back(_("all developers who contributed via Github"));
    entries.push_back(entry);

    // alphabetical again
    entry = CreditsEntry(_("Additional Graphics:"));
    entry.lines.push_back("Marcus Bullin (Parasit)");
    entries.push_back(entry);

    // alphabetical again
    entry = CreditsEntry(_("Additional Support:"));
    entry.lines.push_back("Fenan");
    entry.lines.push_back("Phil Groenewold (Phil333)");
    entry.lines.push_back("muhahahaha");
    entry.lines.push_back("Sotham");
    entry.lines.push_back("Marc Vester (xaser)");
    entries.push_back(entry);

    const std::vector<std::string> donators = {
      _("various anonymous donators"),
      // A                              // 
      "Bob Kromonos Achten",            // 
      "Alles Adam",                     // 
      "Niklas Anders",                  // 
      "Christian Arpe",                 // 
      // B                              // 
      "Karsten Backhaus (K-Duke)",      // 
      "Günter Baumann",                 // 
      "Felix Becker",                   // 
      "Markus Becker",                  // 
      "Sebastian Bernhard",             // 
      "Gilles Bordelais",               // 
      "André Brem",                     // 
      "Hannes Brüske",                  // 
      "Andreas Brüske",                 // 
      // C                              // 
      // D                              // 
      "Jannes Dirks",                   // 
      // E                              // 
      "Gerrit Eberhardt",               // 
      // F                              // 
      "Alexander Faber",                // 
      "Niklas Faig",                    // 
      "Christopher Flocke",             // 
      "Christopher Funke-Kaiser",       // 
      // G                              // 
      "Hans Gabathuler",                // 
      "Thomas Georg",                   // 
      "Konrad Greinke",                 // 
      "Stefan Gunkel",                  // 
      // H                              // 
      "Patrick Haak (Demophobie)",      // 
      "Marius Hacker",                  // 
      "Daniel Hampf",                   // 
      "Nathan Hall",                    // 
      "Christoph Hartmann",             // 
      "Andreas Hauer",                  // 
      "Stephan Hesse",                  // 
      "Daniel Holle",                   // 
      "Rene Hopf",                      // 
      "Hanso Hunder",                   // 
      "Benjamin Hünig",                 // 
      // I                              // 
      // J                              // 
      "Dominic Jonas",                  // 
      "Simon Jais",                     // 
      // K                              // 
      "Silvio Karbe",                   // 
      "Ralli Kasikas",                  // 
      "Jörg Kesten",                    // 
      "Thorsten Kindel",                // 
      "Holger Klötzner",                // 
      "Andreas Kniep",                  // 
      "Vladislav Kolaja",               // 
      "Daniel Krsiak",                  // 
      "Andreas Krimm",                  // 
      "Christopher Kuehnel (Spikeone)", // 
      // L                              // 
      "Alexander Lambio",               // 
      "Oliver Lang",                    // 
      "Marius Loewe",                   // 
      "Eric Lutter",                    // 
      // M                              // 
      "Jan Montag",                     // 
      "Kai Müller",                     // 
      "morlock",                        // 
      "Jan Mozgawa",                    // 
      // N                              // 
      // O                              // 
      // P                              // 
      "Wojciech Pieklik",               // 
      "Mike Plackowski",                // 
      "Daniel Platt",                   // 
      // Q                              // 
      // R                              // 
      "Philip Rau",                     // 
      "Ronny Richter",                  // 
      // S                              // 
      "Daniel Seindl",                  // 
      "Vasilyev Sergey",                // 
      "Patrick Schefczyk",              // 
      "Marcel Schneider",               // 
      "Alexander Schoedon",             // 
      "Max Skuratov",                   // 
      "Philipp Strathausen",            // 
      "Benjamin Stoisch",               // 
      "Felix Stolle",                   // 
      // T                              // 
      "Nina Tillmann",                  // 
      "Angelo Tiegs",                   // 
      // U                              // 
      // V                              // 
      // W                              // 
      "Niels Wiederanders",             // 
      "Philipp Wohlleben"               // 
      // X                              // 
      // Y                              // 
      // Z                              // 
    };

    // alphabetical again, two rows
    entry = CreditsEntry(_("Donators"), _("Thank you for your donations!"));

    const size_t page_size = 15;
    const size_t middle = std::min(page_size, donators.size()) / 2;
    size_t pos = 0;
    for(const auto& donator : donators)
    {
        const size_t column = (pos < middle ? 0 : 1);
        entry.lines.push_back(CreditsEntry::Line(donator, column ));
        ++pos;
        // create new page
        if(pos > page_size) 
        {
            entries.push_back(entry);
            entry.lines.clear();
            pos = 0;
        }
    }

    if(!entry.lines.empty())
    {
        entries.push_back(entry);
    }

    entry = CreditsEntry(_("We hope you enjoy playing Return To The Roots!"), _("THE END"));
    entry.lines.push_back(_("Thank you!"));
    entries.push_back(entry);

    WorldDescription worldDesc;
    GameDataLoader gdLoader(worldDesc);
    if(!gdLoader.Load())
        throw std::runtime_error("Failed to load game data");

    std::vector<bool> nations(NUM_NATIVE_NATS, true);

    LOADER.LoadFilesAtGame(worldDesc.get(DescIdx<LandscapeDesc>(0)).mapGfxPath, false, nations);

    this->itCurEntry = entries.begin();

    if(LOADER.sng_lst.size() > 8)
        LOADER.sng_lst[8]->Play(0);
}

dskCredits::~dskCredits() = default;

void dskCredits::Draw_()
{
    Desktop::Draw_();
    DrawBobs();
    DrawCredit();
}

void dskCredits::GotoNextPage()
{
    // Next page
    ++this->itCurEntry;
    if(this->itCurEntry == entries.end())
        this->itCurEntry = entries.begin();
    startTime = VIDEODRIVER.GetTickCount();
}

void dskCredits::GotoPrevPage()
{
    // Prev page
    if(this->itCurEntry == entries.begin())
        this->itCurEntry = std::prev(entries.end());
    else
        --this->itCurEntry;
    startTime = VIDEODRIVER.GetTickCount();
}

void dskCredits::DrawCredit()
{
    unsigned time = VIDEODRIVER.GetTickCount() - startTime;
    if(time > PAGE_TIME)
    {
        GotoNextPage();
    }

    // calculate text transparency
    unsigned transparency = 0xFF;
    if(time < FADING_TIME)
        transparency = 0xFF * time / FADING_TIME;
    if(time > PAGE_TIME - FADING_TIME)
        transparency = (0xFF - 0xFF * (time - (PAGE_TIME - FADING_TIME)) / FADING_TIME);

    // draw text
    LargeFont->Draw(DrawPoint(40, 100), itCurEntry->title, FontStyle{}, SetAlpha(COLOR_RED, transparency));

    std::array<unsigned, 2> columnToY = {{150, 150}};

    for(auto& line : itCurEntry->lines)
    {
        LargeFont->Draw(DrawPoint(60 + line.column * 350, columnToY[line.column]), line.line, FontStyle{},
                        SetAlpha(COLOR_YELLOW, transparency));
        columnToY[line.column] += LargeFont->getHeight() + 5;
    }

    LargeFont->Draw(DrawPoint(40, columnToY[0] + 20), itCurEntry->lastLine, FontStyle{}, SetAlpha(COLOR_RED, transparency));

    if(itCurEntry->pic)
        itCurEntry->pic->DrawFull(DrawPoint(VIDEODRIVER.GetRenderSize().x - 300, 70), SetAlpha(COLOR_WHITE, transparency));
}

void dskCredits::DrawBobs()
{
    // Frameratebegrenzer
    int msSinceLastBobAnim = VIDEODRIVER.GetTickCount() - bobTime;
    int bobAnimStepsPerSec = 25;

    int msSinceLastBobSpawn = VIDEODRIVER.GetTickCount() - bobSpawnTime;
    int bob_spawnprosec = 5;

    if(VIDEODRIVER.GetFPS() < 30)
        bob_spawnprosec = 0;
    else if(VIDEODRIVER.GetFPS() < 60)
        bob_spawnprosec = 1;
    else if(VIDEODRIVER.GetFPS() < 200)
        bob_spawnprosec = 2;

    // add new bob
    if(bob_spawnprosec > 0 && msSinceLastBobSpawn > (1000 / bob_spawnprosec)
       && (int)bobs.size() < (int)(50 + VIDEODRIVER.GetRenderSize().x / 2))
    {
        bobSpawnTime = VIDEODRIVER.GetTickCount();

        Bob b = Bob();
        b.animationStep = 0;
        b.speed = 1 + rand() % 4;

        // links oder rechts spawnen
        if(rand() % 2 == 0)
        {
            b.pos.x = 0;
            b.direction = 3;
        } else
        {
            b.pos.x = VIDEODRIVER.GetRenderSize().x;
            b.direction = 6;
        }

        b.color = PLAYER_COLORS[rand() % PLAYER_COLORS.size()];
        unsigned job = rand() % 29;

        // exclude "headless" bobs
        if(job == 8 || job == 9 || job == 12 || job == 18)
        {
            job = rand() % (NUM_WARE_TYPES - 1);
            b.hasWare = true;
        } else
        {
            if(job == JOB_SCOUT)
                job = 35 + NATION_RTTR_TO_S2[rand() % 4] * 6;
            else if(job >= JOB_PRIVATE && job <= JOB_GENERAL)
                job = 30 + NATION_RTTR_TO_S2[rand() % 4] * 6 + job - JOB_PRIVATE;
            else
                job = JOB_CONSTS[job].jobs_bob_id;
            b.hasWare = false;
        }

        b.id = job;
        b.pos.y = GetCtrl<ctrlButton>(0)->GetPos().y - 20 - rand() % 150;
        bobs.push_back(b);
    }

    // draw bobs
    for(auto& bob : bobs)
    {
        if(!bob.hasWare)
            LOADER.GetBobN("jobs")->Draw(bob.id, bob.direction, bob.isFat, bob.animationStep, bob.pos, bob.color);
        else
            LOADER.GetBobN("carrier")->Draw(bob.id, bob.direction, bob.isFat, bob.animationStep, bob.pos, bob.color);

        if(msSinceLastBobAnim > (1000 / bobAnimStepsPerSec))
        {
            bobTime = VIDEODRIVER.GetTickCount();

            bob.animationStep++;
            if(bob.animationStep > 7)
                bob.animationStep = 0;
            if(bob.direction == 3)
            {
                bob.pos.x += bob.speed;
                if(bob.pos.x > static_cast<int>(VIDEODRIVER.GetRenderSize().x))
                    bob.direction = 6;
            } else if(bob.direction == 6)
            {
                bob.pos.x -= bob.speed;
                if(bob.pos.x < 0)
                    bob.direction = 3;
            }
        }
    }

    // Frameratebegrenzer aktualisieren
    if(msSinceLastBobAnim > (1000 / bobAnimStepsPerSec))
        bobTime = VIDEODRIVER.GetTickCount();
}

bool dskCredits::Close()
{
    WINDOWMANAGER.Switch(std::make_unique<dskMainMenu>());
    return true;
}

glArchivItem_Bitmap* dskCredits::GetCreditsImgOrDefault(const std::string& name)
{
    glArchivItem_Bitmap* result = LOADER.GetImage("credits", name + ".bmp");
    if(!result)
        result = LOADER.GetImage("credits", "default.bmp");
    return result;
}

bool dskCredits::Msg_KeyDown(const KeyEvent& /*ke*/)
{
    GotoNextPage();
    return true;
}

bool dskCredits::Msg_LeftUp(const MouseCoords& /*mc*/)
{
    GotoNextPage();
    return true;
}

bool dskCredits::Msg_RightUp(const MouseCoords& mc)
{
    GotoPrevPage();
    return true;
}

void dskCredits::Msg_ButtonClick(const unsigned /*ctrl_id*/)
{
    Close();
}

void dskCredits::SetActive(bool active)
{
    Desktop::SetActive(active);
    if(active)
    {
        startTime = bobTime = bobSpawnTime = VIDEODRIVER.GetTickCount();
    }
}
