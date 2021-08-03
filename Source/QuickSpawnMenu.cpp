/**
    bespoke synth, a software modular synthesizer
    Copyright (C) 2021 Ryan Challinor (contact: awwbees@gmail.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/
/*
  ==============================================================================

    QuickSpawnMenu.cpp
    Created: 22 Oct 2017 7:49:17pm
    Author:  Ryan Challinor

  ==============================================================================
*/

#include "QuickSpawnMenu.h"
#include "ModularSynth.h"
#include "ModuleFactory.h"

QuickSpawnMenu* TheQuickSpawnMenu = nullptr;

namespace
{
   const int itemSpacing = 15;
   ofVec2f moduleGrabOffset(-40,10);
}

QuickSpawnMenu::QuickSpawnMenu()
: mLastHoverX(0)
, mLastHoverY(0)
{
   assert(TheQuickSpawnMenu == nullptr);
   TheQuickSpawnMenu = this;
}

QuickSpawnMenu::~QuickSpawnMenu()
{
   assert(TheQuickSpawnMenu == this);
   TheQuickSpawnMenu = nullptr;
}

void QuickSpawnMenu::Init()
{
   IDrawableModule::Init();
   SetShouldDrawOutline(false);
   SetShowing(false);
}

void QuickSpawnMenu::KeyPressed(int key, bool isRepeat)
{
   IDrawableModule::KeyPressed(key, isRepeat);
   if (((key >= 'a' && key <= 'z') || key == ';') && !isRepeat && GetKeyModifiers() == kModifier_None)
   {
      mElements = TheSynth->GetModuleFactory()->GetSpawnableModules((char)key);
      if (mElements.size() > 0)
      {
         mCurrentMenuChar = (char)key;

         float width = 150;
         for (auto element : mElements)
         {
            float elementWidth = GetStringWidth(element) + 10;
            if (elementWidth > width)
               width = elementWidth;
         }
         
         SetDimensions(width, (int)mElements.size() * itemSpacing);
         SetPosition(TheSynth->GetMouseX(GetOwningContainer())-mWidth/2, TheSynth->GetMouseY(GetOwningContainer())-mHeight/2);
         SetShowing(true);
      }
   }
}

void QuickSpawnMenu::KeyReleased(int key)
{
   if (key == mCurrentMenuChar)
   {
      SetShowing(false);
   }
}

void QuickSpawnMenu::MouseReleased()
{
   if (IsShowing())
   {
      SetShowing(false);
   }
}

void QuickSpawnMenu::DrawModule()
{
   ofPushStyle();
   
   int highlightIndex = -1;
   if (TheSynth->GetMouseY(GetOwningContainer()) > GetPosition().y)
      highlightIndex = (TheSynth->GetMouseY(GetOwningContainer()) - GetPosition().y)/itemSpacing;
   
   ofSetColor(50,50,50,100);
   ofFill();
   ofRect(-2,-2,mWidth+4,mHeight+4);
   for (int i=0; i<mElements.size(); ++i)
   {
      ofSetColor(IDrawableModule::GetColor(TheSynth->GetModuleFactory()->GetModuleType(mElements[i]))*(i==highlightIndex ? .7f : .5f), 255);
      ofRect(0, i*itemSpacing+1, mWidth, itemSpacing-1);
      if (i == highlightIndex)
         ofSetColor(255, 255, 0);
      else
         ofSetColor(255,255,255);
      DrawTextNormal(mElements[i], 1, i*itemSpacing+12);
   }
   
   ofPopStyle();
}

bool QuickSpawnMenu::MouseMoved(float x, float y)
{
   mLastHoverX = x;
   mLastHoverY = y;
   return false;
}

void QuickSpawnMenu::OnClicked(int x, int y, bool right)
{
   if (right)
      return;
   
   string moduleTypeName = GetModuleTypeNameAt(x, y);
   if (moduleTypeName != "")
   {
      IDrawableModule* module = TheSynth->SpawnModuleOnTheFly(moduleTypeName, TheSynth->GetMouseX(GetOwningContainer()) + moduleGrabOffset.x, TheSynth->GetMouseY(GetOwningContainer()) + moduleGrabOffset.y);
      TheSynth->SetMoveModule(module, moduleGrabOffset.x, moduleGrabOffset.y);
   }
   
   SetShowing(false);
}

string QuickSpawnMenu::GetHoveredModuleTypeName()
{
   return GetModuleTypeNameAt(mLastHoverX, mLastHoverY);
}

string QuickSpawnMenu::GetModuleTypeNameAt(int x, int y)
{
   int index = y / itemSpacing;
   if (index >= 0 && index < mElements.size())
      return mElements[index];

   return "";
}
