
//    Scorched3D (c) 2000-2011
//
//    This file is part of Scorched3D.
//
//    Scorched3D is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    Scorched3D is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
////////////////////////////////////////////////////////////////////////////////

#include <engine/GameState.h>
#include <engine/GameStateI.h>
#include <engine/GameStateStimulusI.h>
#include <engine/MainLoop.h>
#include <common/Keyboard.h>
#include <common/Defines.h>
#include <common/Logger.h>
#include <common/LoggerI.h>
#include <limits.h>
#include <SDL/SDL.h>

GameStatePerfCounter::GameStatePerfCounter(const char *name) : 
	name_(name), total_(0), used_(false)
{
}

GameStatePerfCounter::~GameStatePerfCounter()
{
}

void GameStatePerfCounter::start()
{
	used_ = true;
	start_ = SDL_GetTicks();
}

void GameStatePerfCounter::end()
{
	used_ = true;
	unsigned int end = SDL_GetTicks();
	total_ += end - start_;
}

unsigned int GameStatePerfCounter::getTotal()
{ 
	unsigned int lastTotal = total_;
	total_ = 0;
	used_ = false;
	return lastTotal; 
}

GameState::GameState(MainLoop *mainLoop, const char *name) :
	name_(name),
	fakeMiddleButton_(true),
	currentMouseState_(0),
	currentState_(UINT_MAX),
	currentEntry_(0),
	stateCount_(0),
	currentStateI_(0),
	currentMouseX_(0), currentMouseY_(0),
	mouseDoubleX_(0), mouseDoubleY_(0),
	stateLogging_(false), 
	stateTimeLogging_(false), 
	frameTime_(0.0f), frameCount_(0),
	mainLoop_(mainLoop)
{
	clearTimers();
}

GameState::~GameState()
{

}

void GameState::clear()
{
	stateList_.clear();
}

void GameState::setFakeMiddleButton(bool fake)
{
	fakeMiddleButton_ = fake;
}

void GameState::mouseWheel(short z)
{
	bool skipRest = false;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;
		unsigned thisStateCount = stateCount_;

		StateIList *currentList = &thisEntry->subMouseWheelList;
		if (!currentList->empty())
		{
			StateIList::iterator subItor;
			for (subItor = currentList->begin();
				subItor != currentList->end();
				++subItor)
			{
				(*subItor)->mouseWheel(thisState, 
					currentMouseX_, currentMouseY_, 
					(int) z, skipRest);
				if (thisStateCount != stateCount_) return;
				if (skipRest) break;
			}
		}
	}
}

void GameState::mouseMove(int x, int y)
{
	currentMouseX_ = x;
	currentMouseY_ = y;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		if (MouseButtonMiddle & currentMouseState_)
		{
			int diffX = x - mouseMDragX_; mouseMDragX_ = x;
			int diffY = mouseMDragY_ - y; mouseMDragY_ = y;

			mouseMoveCall(thisState, MouseButtonMiddle, 
						  thisEntry->subMouseDragMiddleList,
					x, y, diffX, diffY);
		}

		if (fakeMiddleButton_ && (MouseButtonLeft & currentMouseState_ && 
								  MouseButtonRight & currentMouseState_))
		{
			int diffX = x - mouseMDragX_; mouseMDragX_ = x;
			int diffY = mouseMDragY_ - y; mouseMDragY_ = y;

			mouseMoveCall(thisState, MouseButtonMiddle, 
						  thisEntry->subMouseDragMiddleList,
					x, y, diffX, diffY);
		}
		else
		{
			if (MouseButtonLeft & currentMouseState_)
			{
				int diffX = x - mouseLDragX_; mouseLDragX_ = x;
				int diffY = mouseLDragY_ - y; mouseLDragY_ = y;

				mouseMoveCall(thisState, MouseButtonLeft, 
							  thisEntry->subMouseDragLeftList,
						x, y, diffX, diffY);
			}
			if (MouseButtonRight & currentMouseState_)
			{
				int diffX = x - mouseRDragX_; mouseRDragX_ = x;
				int diffY = mouseRDragY_ - y; mouseRDragY_ = y;

				mouseMoveCall(thisState, MouseButtonRight, 
							  thisEntry->subMouseDragRightList,
						x, y, diffX, diffY);
			}
		}
	}
}

void GameState::mouseMoveCall(const unsigned state, MouseButton button, 
							  StateIList &currentList, 
							  int mx, int my,
							  int dx, int dy)
{
	if (!currentList.empty())
	{
		bool skipRest = false;
		StateIList::iterator subItor;
		for (subItor = currentList.begin();
			subItor != currentList.end();
			++subItor)
		{
			(*subItor)->mouseDrag(state, button, mx, my, dx, dy, skipRest);
			if (skipRest) break;
		}
	}
}

void GameState::mouseDown(MouseButton button, int x, int y)
{
	mouseUpDown(button, true, x, y);

	if (doubleClickClock_.getTimeDifference() < 0.25 &&
		abs(mouseDoubleX_ - x) <= 4 &&
		abs(mouseDoubleY_ - y) <= 4)
	{
		switch (button)
		{
		case MouseButtonLeft:
			mouseUpDown(MouseButtonLeftDoubleClick, true, x, y);
			break;
		case MouseButtonMiddle:
			mouseUpDown(MouseButtonMiddleDoubleClick, true, x, y);
			break;
		case MouseButtonRight:
			mouseUpDown(MouseButtonRightDoubleClick, true, x, y);
			break;
		}
	}
	mouseDoubleX_ = x;
	mouseDoubleY_ = y;
}

void GameState::mouseUp(MouseButton button, int x, int y)
{
	mouseUpDown(button, false, x, y);
}

void GameState::mouseUpDown(MouseButton button, bool down, int x, int y)
{
	bool skipRest = false;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;
		unsigned thisStateCount = stateCount_;

		StateIList *currentList = 0;
		if (down)
		{
			if (button <= MouseButtonLeftDoubleClick)
			{
				currentMouseState_ |= (unsigned) button;
			}
			switch(button)
			{
			case MouseButtonRight:
				mouseRDragX_ = x; mouseRDragY_ = y;
				currentList = &thisEntry->subMouseDownRightList;
				break;
			case MouseButtonMiddle:
				mouseMDragX_ = x; mouseMDragY_ = y;
				currentList = &thisEntry->subMouseDownMiddleList;
				break;
			default:
				mouseLDragX_ = x; mouseLDragY_ = y;
				currentList = &thisEntry->subMouseDownLeftList;
				break;
			}

			if (fakeMiddleButton_ && 
				(MouseButtonLeft & currentMouseState_ && 
				MouseButtonRight & currentMouseState_))
			{
				mouseMDragX_ = x;
				mouseMDragY_ = y;
			}
		}
		else
		{
			if (fakeMiddleButton_ && 
				(MouseButtonLeft & currentMouseState_ && 
				MouseButtonRight & currentMouseState_))
			{
				currentList = &thisEntry->subMouseUpMiddleList;
			}
			else
			{
				switch(button)
				{
				case MouseButtonRight:
					currentList = &thisEntry->subMouseUpRightList;
					break;
				case MouseButtonMiddle:
					currentList = &thisEntry->subMouseUpMiddleList;
					break;
				default:
					currentList = &thisEntry->subMouseUpLeftList;
					break;
				}
			}

			if (button <= MouseButtonLeftDoubleClick)
			{
				currentMouseState_ ^= (unsigned) button;
			}
		}

		if (!currentList->empty())
		{
			StateIList::iterator subItor;
			for (subItor = currentList->begin();
				subItor != currentList->end();
				++subItor)
			{
				if (down) 
				{
					(*subItor)->mouseDown(thisState, button, x, y, skipRest);
				}
				else 
				{
					(*subItor)->mouseUp(thisState, button, x, y, skipRest);
				}
				if (thisStateCount != stateCount_) return;
				if (skipRest) break;
			}
		}
	}
}

void GameState::simulate(float simTime)
{
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;
		unsigned thisStateCount = stateCount_;

		if (stateTimeLogging_)
		{
			frameTime_ += simTime;
			if (frameTime_ > 10.0f) clearTimers(true);
		}

		timerClock_.getTicksDifference();
		int timerCount = 0;

		std::list<GameStateSubEntry>::iterator itor;
		for (itor = thisEntry->loopList.begin();
			itor != thisEntry->loopList.end();
			++itor)
		{
			StateIList::iterator subItor;
			for (subItor = itor->subLoopList.begin();
				subItor != itor->subLoopList.end();
				++subItor, timerCount++)
			{
				GameStateI *stateI = (*subItor);
				currentStateI_ = stateI;
				stateI->simulate(thisState, simTime);
				if (thisStateCount != stateCount_) return;

				timers_[timerCount % 50].simulateTime += 
					timerClock_.getTicksDifference();
				timers_[timerCount % 50].gameStateI = stateI;
			}

			// Draw after all other elements as the camera
			// needs to up to date with respect to all other
			// changes made in the simulate loop
			itor->current->simulate(thisState, simTime);
			if (thisStateCount != stateCount_) return;
		}

		if (!thisEntry->subKeyList.empty())
		{
			unsigned int historySize;
			unsigned int bufferSize;

			char *buffer  = 
				Keyboard::instance()->getkeyboardbuffer(bufferSize);
			KeyboardHistory::HistoryElement *history = 
				Keyboard::instance()->getkeyboardhistory(historySize);
			unsigned int keyState = 
				Keyboard::instance()->getKeyboardState();

			bool skipRest = false;
			StateIList::iterator subItor;
			for (subItor = thisEntry->subKeyList.begin();
				subItor != thisEntry->subKeyList.end();
				++subItor)
			{
				(*subItor)->keyboardCheck(thisState, simTime, buffer, keyState, 
					history, historySize, skipRest);
				if (thisStateCount != stateCount_) return;
				if (skipRest) break;
			}
		}

		if (!thisEntry->condStimList.empty())
		{
			StiulusIList::iterator itor;
			for (itor = thisEntry->condStimList.begin();
				itor != thisEntry->condStimList.end();
				++itor)
			{
				SimulusIPair &p = *itor;
				if (p.first->acceptStateChange(thisState, p.second, simTime))
				{
					if (stateLogging_)
					{
						Logger::log(S3D::formatStringBuffer("%s::acceptStateChange(%i, %i)", 
							name_.c_str(), thisState, p.second));
					}
					setState(itor->second);
					return;
				}
				if (thisStateCount != stateCount_) return;
			}
		}
	}
}

void GameState::draw()
{
	frameCount_++;
	if (currentEntry_)
	{
		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;
		unsigned thisStateCount = stateCount_;

		timerClock_.getTicksDifference();
		int timerCount = 0;

		std::list<GameStateSubEntry>::iterator itor;
		for (itor = thisEntry->loopList.begin();
			itor != thisEntry->loopList.end();
			++itor)
		{
			itor->current->draw(thisState);

			StateIList::iterator subItor;
			for (subItor = itor->subLoopList.begin();
				subItor != itor->subLoopList.end();
				++subItor, timerCount++)
			{
				GameStateI *stateI = (*subItor);
				currentStateI_ = stateI;
				stateI->draw(thisState);
				if (thisStateCount != stateCount_) return;

				timers_[timerCount % 50].drawTime += 
					timerClock_.getTicksDifference();
				timers_[timerCount % 50].gameStateI = stateI;
			}
		}
	}
}

void GameState::setState(const unsigned state)
{
	if (stateLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::setState(%i)", name_.c_str(), state));
	}

	clearTimers();

	stateCount_++;
	currentState_ = state;
	currentEntry_ = 0;
	std::map<unsigned, GameStateEntry>::iterator itor = stateList_.find(state);
	if (itor != stateList_.end())
	{
		currentEntry_ = &itor->second;

		GameStateEntry *thisEntry = currentEntry_;
		unsigned thisState = currentState_;

		if (!thisEntry->enterStateList.empty())
		{
			StateIList::iterator subItor;
			for (subItor = thisEntry->enterStateList.begin();
				subItor != thisEntry->enterStateList.end();
				++subItor)
			{
				GameStateI *s = (*subItor);
				s->enterState(thisState);
			}
		}
	}
	else
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
			"%s: Failed to find state %i", 
			name_.c_str(),
			state));
	}

	if (stateLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::setStateFinished(%i)", name_.c_str(), state));
	}
}

void GameState::stimulate(const unsigned stimulus)
{
	if (stateLogging_)
	{
		Logger::log(S3D::formatStringBuffer("%s::stimulate(%i)", name_.c_str(), stimulus));
	}

	std::map<unsigned, unsigned>::iterator itor = 
		currentEntry_->stimList.find(stimulus);
	if (itor != currentEntry_->stimList.end())
	{
		setState(itor->second);
		return;
	}
	else
	{
		S3D::dialogExit("Scorched3D", S3D::formatStringBuffer(
			"%s: Failed to find stimulus %i in state %i", 
			name_.c_str(),
			stimulus, currentState_));
	}
}

GameState::GameStateEntry* GameState::getEntry(const unsigned state)
{
	GameStateEntry *foundEntry = 0;
	std::map<unsigned, GameStateEntry>::iterator itor = stateList_.find(state);
	if (itor == stateList_.end())
	{
		GameStateEntry newEntry;
		stateList_[state] = newEntry;

		foundEntry = &stateList_[state];

		if (currentState_ == state)
		{
			currentEntry_ = &stateList_[state];
		}
	}
	else
	{
		foundEntry = &itor->second;
	}

	return foundEntry;
}

GameState::GameStateSubEntry* GameState::getSubEntry(const unsigned state, 
													 GameStateI *entry)
{
	GameStateEntry *foundEntry = getEntry(state);

	GameStateSubEntry *foundSubEntry = 0;
	std::list<GameStateSubEntry>::iterator subItor;
	for (subItor = foundEntry->loopList.begin();
		subItor != foundEntry->loopList.end();
		++subItor)
	{
		if (subItor->current == entry)
		{
			foundSubEntry = &(*subItor);
		}
	}

	if (!foundSubEntry)
	{
		GameStateSubEntry newEntry;
		newEntry.current = entry;

		foundEntry->loopList.push_back(newEntry);
		foundSubEntry = &foundEntry->loopList.back();
	}

	return foundSubEntry;
}

void GameState::addStateLoop(const unsigned state, GameStateI *entry, 
							 GameStateI *subEntry)
{
	getSubEntry(state, entry)->subLoopList.push_back(subEntry);
}

void GameState::addStateEntry(const unsigned state, GameStateI *subEntry)
{
	getEntry(state)->enterStateList.push_back(subEntry);
}

void GameState::addStateKeyEntry(const unsigned state, GameStateI *subEntry)
{
	getEntry(state)->subKeyList.push_back(subEntry);
}

void GameState::addStateMouseDownEntry(const unsigned state, 
									   const unsigned buttons, 
									   GameStateI *subEntry)
{
	if (buttons & MouseButtonLeft)
	{
		getEntry(state)->subMouseDownLeftList.push_back(subEntry);
	}
	if (buttons & MouseButtonRight)
	{
		getEntry(state)->subMouseDownRightList.push_back(subEntry);
	}
	if (buttons & MouseButtonMiddle)
	{
		getEntry(state)->subMouseDownMiddleList.push_back(subEntry);
	}
}

void GameState::addStateMouseUpEntry(const unsigned state, 
									 const unsigned buttons, 
									 GameStateI *subEntry)
{
	if (buttons & MouseButtonLeft)
	{
		getEntry(state)->subMouseUpLeftList.push_back(subEntry);
	}
	if (buttons & MouseButtonRight)
	{
		getEntry(state)->subMouseUpRightList.push_back(subEntry);
	}
	if (buttons & MouseButtonMiddle)
	{
		getEntry(state)->subMouseUpMiddleList.push_back(subEntry);
	}
}

void GameState::addStateMouseDragEntry(const unsigned state, 
									   const unsigned buttons, 
									   GameStateI *subEntry)
{
	if (buttons & MouseButtonLeft)
	{
		getEntry(state)->subMouseDragLeftList.push_back(subEntry);
	}
	if (buttons & MouseButtonRight)
	{
		getEntry(state)->subMouseDragRightList.push_back(subEntry);
	}
	if (buttons & MouseButtonMiddle)
	{
		getEntry(state)->subMouseDragMiddleList.push_back(subEntry);
	}
}

void GameState::addStateMouseWheelEntry(const unsigned state, 
										GameStateI *subEntry)
{
	getEntry(state)->subMouseWheelList.push_back(subEntry);
}

void GameState::addStateStimulus(const unsigned state, 
								 const unsigned stim, 
								 const unsigned nexts)
{
	GameState::GameStateEntry *entry = getEntry(state);
	std::map<unsigned, unsigned>::iterator itor = entry->stimList.find(stim);
	if (itor == entry->stimList.end())
	{
		entry->stimList[stim] = nexts;
	}
	else
	{
		DIALOG_ASSERT(0);
	}
}

void GameState::addStateStimulus(const unsigned state, 
								 GameStateStimulusI *check, 
								 const unsigned nexts)
{
	GameState::GameStateEntry *entry = getEntry(state);
	SimulusIPair pair(check, nexts);
	entry->condStimList.push_back(pair);
}

void GameState::clearTimers(bool printTimers)
{
	unsigned int sinceLastTime = overallTimerClock_.getTicksDifference();
	if (printTimers && stateTimeLogging_)
	{
		unsigned int simulateTotal = 0, drawTotal = 0;
		for (int i=0; i<50; i++)
		{
			if (timers_[i].gameStateI)
			{
				simulateTotal += timers_[i].simulateTime;
				drawTotal += timers_[i].drawTime;
			}
		}
		int timeLeft = int(sinceLastTime) - int(drawTotal + simulateTotal);
		if (sinceLastTime == 0) sinceLastTime = 1;

		unsigned int drawTotalPer = (100 * drawTotal) / sinceLastTime;
		unsigned int simulateTotalPer = (100 * simulateTotal) / sinceLastTime;
		unsigned int timeLeftPer =  (100 * timeLeft) / sinceLastTime;

		Logger::log(
			"----------------------------------------");

		Logger::log(S3D::formatStringBuffer("FPS %.2f  Time Total %.2f = Draw %.2f + Clear %.2f",
			float(frameCount_) / mainLoop_->getTotalTime(),
			mainLoop_->getTotalTime(), mainLoop_->getDrawTime(), mainLoop_->getClearTime()));
		mainLoop_->getTotalTime() = 0.0f;
		mainLoop_->getDrawTime() = 0.0f;
		mainLoop_->getClearTime() = 0.0f;

		Logger::log(S3D::formatStringBuffer("%10s Other : %4i (%3u%%) Draw : %4u (%3u%%), Simulate : %4u (%3u%%)\n\n", 
			"",
			timeLeft, timeLeftPer,
			drawTotal, drawTotalPer,
			simulateTotal, simulateTotalPer));
		for (int i=0; i<50; i++)
		{
			if (timers_[i].gameStateI)
			{
				unsigned int percentageSimulate =
					(100 * timers_[i].simulateTime) / sinceLastTime;
				unsigned int percentageDraw =
					(100 * timers_[i].drawTime) / sinceLastTime;
				if (percentageSimulate > 0 || percentageDraw > 0)
				{
					Logger::log(S3D::formatStringBuffer("%2i:%25s - Draw : %4u (%3u%%), Simulate : %4u (%3u%%)", 
						i, 
						timers_[i].gameStateI->getGameStateIName(),
						timers_[i].drawTime, percentageDraw,
						timers_[i].simulateTime, percentageSimulate));
				}

				std::vector<GameStatePerfCounter *>::iterator itor;
				for (itor = timers_[i].gameStateI->getPerfCounters().begin();
					itor != timers_[i].gameStateI->getPerfCounters().end();
					++itor)
				{
					GameStatePerfCounter *counter = *itor;
					bool used = counter->getUsed();
					int total = counter->getTotal();
					if (used && total > 0 &&
						(percentageSimulate > 0 || percentageDraw > 0))
					{
						Logger::log(S3D::formatStringBuffer("%35s +- %4u", 
							counter->getName(),
							total));
					}
				}
			}
		}
	}

	memset(&timers_, 0, sizeof(timers_));
	frameTime_ = 0.0f;
	frameCount_ = 0;
}

int GameState::getPerfCounter(const char *name)
{
	DIALOG_ASSERT(currentStateI_);
	return currentStateI_->getPerfCounter(name);
}

void GameState::startPerfCount(int counter)
{
	DIALOG_ASSERT(currentStateI_);
	currentStateI_->startPerfCount(counter);
}

void GameState::endPerfCount(int counter)
{
	DIALOG_ASSERT(currentStateI_);
	currentStateI_->endPerfCount(counter);
}
