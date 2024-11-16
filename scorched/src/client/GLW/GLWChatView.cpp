////////////////////////////////////////////////////////////////////////////////
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

#include <GLW/GLWChatView.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWPanel.h>
#include <GLW/GLWColors.h>
#include <GLEXT/GLState.h>
#include <graph/OptionsDisplay.h>
#include <image/ImageFactory.h>
#include <XML/XMLParser.h>
#include <common/Keyboard.h>
#include <common/ToolTipResource.h>
#include <client/ClientChannelManager.h>
#include <client/ScorchedClient.h>
#include <target/TargetContainer.h>
#include <lang/LangResource.h>
#include <lang/LangParam.h>

REGISTER_CLASS_SOURCE(GLWChatView);

GLWChatView::GLWChatView(float x, float y, float w, float h) : 
	GLWidget(x, y, w, h),
	init_(false), 
	visibleLines_(5), totalLines_(50),
	displayTime_(10.0f),
	fontSize_(12.0f), outlineFontSize_(14.0f), lineDepth_(18),
	currentVisible_(0), 
	alignTop_(false), parentSized_(false), splitLargeLines_(false),
	scrollPosition_(-1), allowScroll_(false), 
	upButton_(x_ + 2.0f, y_ + 1.0f, 12.0f, 12.0f),
	downButton_(x_ + 2.0f, y_ + 1.0f, 12.0f, 12.0f),
	resetButton_(x_ + 2.0f, y_ + 1.0f, 14.0f, 14.0f),
	scrollUpKey_(0), scrollDownKey_(0), scrollResetKey_(0)
{
	setX(x_);
	setY(y_);

	upButton_.setHandler(this);
	downButton_.setHandler(this);
	resetButton_.setHandler(this);

	upButton_.setToolTip(new ToolTipResource(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"CHAT_PREVIOUS", "Chat Previous", 
		"CHAT_PREVIOUS_TOOLTIP", "Show previous chat entry"));
	downButton_.setToolTip(new ToolTipResource(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"CHAT_NEXT", "Chat Next", 
		"CHAT_NEXT_TOOLTIP", "Show next chat entry"));
	resetButton_.setToolTip(new ToolTipResource(
		ToolTip::ToolTipAlignLeft | ToolTip::ToolTipHelp, 
		"CHAT_LAST", "Chat Last", 
		"CHAT_LAST_TOOLTIP", "View end of the chat log, \n"
		"hide all elapsed entries"));

	upButton_.setTextureImage(ImageID(S3D::eModLocation,"", "data/windows/arrow_u.png"));
	downButton_.setTextureImage(ImageID(S3D::eModLocation,"", "data/windows/arrow_d.png"));
	resetButton_.setTextureImage(ImageID(S3D::eModLocation,"", "data/windows/arrow_s.png"));
}

GLWChatView::~GLWChatView()
{
	clearChat();
}

void GLWChatView::clearChat()
{
	scrollPosition_ = -1;
	while (!textLines_.empty())
	{
		delete textLines_.back();
		textLines_.pop_back();
	}
}

void GLWChatView::buttonDown(unsigned int id)
{
	if (!allowScroll_) return;

	if (id == upButton_.getId())
	{
		scrollPosition_ ++;
		if (scrollPosition_ > (int) textLines_.size() - 1)
			scrollPosition_ = (int) textLines_.size() - 1;
	}
	else if (id == downButton_.getId())
	{
		scrollPosition_ --;
		if (scrollPosition_ < 0) scrollPosition_ = 0;
	}
	else
	{
		scrollPosition_ = -1;
	}
}

int GLWChatView::splitLine(const LangString &message)
{
	int lastSpace = 0;
	int totalLen = (int) message.size();
	for (int len=1; len<totalLen; len++)
	{
		float width = 0.0f;
		if (splitLargeLines_)
		{
			width = GLWFont::instance()->getGameFont()->
				getWidth(outlineFontSize_, message, len);
		}

		if (width > w_)
		{
			// If there is a space within the last 15 characters split to it
			if (lastSpace && (len - lastSpace < 15)) 
			{
				return lastSpace;
			}
			else
			{
				// Else just split where we are now
				return len - 1;
			}
		}

		if (message[len] == '\n')
		{
			return len + 1;
		}
		if (message[len] == ' ')
		{
			lastSpace = len + 1;
		}
	}

	// The whole message fits, split to the end of the message
	return totalLen;
}

void GLWChatView::addLargeChat(const Vector &color, const LangString &text, GLFont2dI *renderer)
{
	int currentLen = 0;
	int totalLen = (int) text.size();
	while (currentLen < totalLen)
	{
		// Get the next split position
		int partLen = splitLine(&text[currentLen]);
		bool nl=(text[currentLen + partLen - 1] == '\n');
		LangString subset(text, currentLen, (nl?partLen-1:partLen));

		// Create the new text and add it
		addChat(color, subset, currentLen==0?renderer:0);

		// Increment the current position
		currentLen += partLen;
	}
}

void GLWChatView::addChat(const Vector &color, const LangString &text, GLFont2dI *renderer)
{
	GLWChatViewEntry *entry = new GLWChatViewEntry(color, text, displayTime_, renderer);
	textLines_.push_front(entry);

	if (scrollPosition_ > 0)
	{
		scrollPosition_ ++;
		if (scrollPosition_ > (int) textLines_.size() - 1)
			scrollPosition_ = (int) textLines_.size() - 1;
	}

	if (textLines_.size() > (unsigned int) totalLines_)
	{
		delete textLines_.back();
		textLines_.pop_back();
	}
}

void GLWChatView::simulate(float frameTime)
{
	currentVisible_ = 0;
	int count = 0;
	std::list<GLWChatViewEntry*>::iterator itor;
	for (itor = textLines_.begin();
		itor != textLines_.end() && count < visibleLines_;
		++itor, count++)
	{
		GLWChatViewEntry &entry = *(*itor);
		entry.timeRemaining -= frameTime;
		if (entry.timeRemaining > 0.0f) currentVisible_ ++;
	}
}

void GLWChatView::setX(float x)
{
	GLWidget::setX(x);
	upButton_.setX(x_ + 1.0f);
	downButton_.setX(x_ + 1.0f);
	resetButton_.setX(x_ + 1.0f);
}

void GLWChatView::setY(float y)
{
	GLWidget::setY(y);
	upButton_.setY(y_ + 36.0f);
	downButton_.setY(y_ + 24.0f);
	resetButton_.setY(y_ + 6.0f);
}

void GLWChatView::draw()
{
	GLWidget::draw();

	if (allowScroll_)
	{
		upButton_.draw();
		downButton_.draw();
		resetButton_.draw();
	}

	if (!init_)
	{
		init_ = true;
		if (parent_ && parentSized_)
		{
			w_ = parent_->getW();
			h_ = parent_->getH();
		}
	}

	// Check if there is anything to draw
	if (textLines_.empty()) return;

	// Find the start of the area to draw to
	GLState currentStateBlend(GLState::TEXTURE_ON | 
		GLState::BLEND_ON | GLState::DEPTH_OFF);
	float start = y_ + 8.0f; //lineDepth_;
	if (alignTop_)
	{
		start = y_ + h_ - float(currentVisible_) * lineDepth_;
	}

	// Skip the lines we are not viewing
	// (Should make this more efficient)
	std::list<GLWChatViewEntry*>::iterator startingPos = textLines_.begin();
	{
		int count = 0;
		for (;
			startingPos != textLines_.end() && count < scrollPosition_;
			++startingPos, count++)
		{
		}
	}

	// Draw all the black outlines first
	// Do this in a block incase any of the outlines would over write any
	// of the other lines
	{
		int count = 0;
		std::list<GLWChatViewEntry*>::iterator itor;
		for (itor = startingPos;
			itor != textLines_.end() && count < visibleLines_;
			++itor, count++)
		{
			GLWChatViewEntry &entry = *(*itor);

			float alpha = 1.0f;
			if (scrollPosition_ < 0)
			{
				if (entry.timeRemaining <= 0.0f) continue;
				if (entry.timeRemaining < 1.0f) alpha = entry.timeRemaining;
			}

			float x = x_ + 20.0f;
			float y = start + count * lineDepth_;

			GLWFont::instance()->getGameShadowFont()->
				drawA(GLWColors::black, alpha, fontSize_,
					x - 1.0f, y + 1.0f, 0.0f, 
					entry.text);
		}
	}
	// Draw the actual text
	{
		int count = 0;
		std::list<GLWChatViewEntry*>::iterator itor;
		for (itor = startingPos;
			itor != textLines_.end() && count < visibleLines_;
			++itor, count++)
		{
			GLWChatViewEntry &entry = *(*itor);

			float alpha = 1.0f;
			if (scrollPosition_ < 0)
			{
				if (entry.timeRemaining <= 0.0f) continue;
				if (entry.timeRemaining < 1.0f) alpha = entry.timeRemaining;
			}

			float x = x_ + 20.0f;
			float y = start + count * lineDepth_;

			if (entry.renderer)
			{
				GLWFont::instance()->getGameFont()->
					drawA(entry.renderer, entry.color, alpha, fontSize_,
						x, y, 0.0f, 
						entry.text);
			} 
			else
			{
				GLWFont::instance()->getGameFont()->
					drawA(entry.color, alpha, fontSize_,
						x, y, 0.0f, 
						entry.text);
			}
		}
	}
}

void GLWChatView::mouseDown(int button, float x, float y, bool &skipRest)
{
	skipRest = false;
	upButton_.mouseDown(button, x, y, skipRest);
	downButton_.mouseDown(button, x, y, skipRest);
	resetButton_.mouseDown(button, x, y, skipRest);
}

void GLWChatView::mouseUp(int button, float x, float y, bool &skipRest)
{
	skipRest = false;
	upButton_.mouseUp(button, x, y, skipRest);
	downButton_.mouseUp(button, x, y, skipRest);
	resetButton_.mouseUp(button, x, y, skipRest);
}

void GLWChatView::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	skipRest = false;
	upButton_.mouseDrag(button, mx, my, x, y, skipRest);
	downButton_.mouseDrag(button, mx, my, x, y, skipRest);
	resetButton_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWChatView::keyDown(char *buffer, unsigned int keyState, 
	KeyboardHistory::HistoryElement *history, int hisCount, 
	bool &skipRest)
{
	if (scrollUpKey_ && scrollUpKey_->keyDown(buffer, keyState, false))
	{
		buttonDown(upButton_.getId());
		skipRest = true;
	}
	if (scrollDownKey_ && scrollDownKey_->keyDown(buffer, keyState, false))
	{
		buttonDown(downButton_.getId());
		skipRest = true;
	}
	if (scrollResetKey_ && scrollResetKey_->keyDown(buffer, keyState, false))
	{
		buttonDown(resetButton_.getId());
		skipRest = true;
	}
}

bool GLWChatView::initFromXML(XMLNode *node)
{
	if (!GLWidget::initFromXML(node)) return false;

	if (!initFromXMLInternal(node)) return false;
		
	return true;
}

bool GLWChatView::initFromXMLInternal(XMLNode *node)
{
	if (!node->getNamedChild("displaytime", displayTime_)) return false;
	if (!node->getNamedChild("totallines", totalLines_)) return false;
	if (!node->getNamedChild("visiblelines", visibleLines_)) return false;
	if (!node->getNamedChild("linedepth", lineDepth_)) return false;
	if (!node->getNamedChild("fontsize", fontSize_)) return false;
	if (!node->getNamedChild("outlinefontsize", outlineFontSize_)) return false;
	if (!node->getNamedChild("textaligntop", alignTop_)) return false;
	if (!node->getNamedChild("parentsized", parentSized_)) return false;
	if (!node->getNamedChild("splitlargelines", splitLargeLines_)) return false;
	if (!node->getNamedChild("allowscroll", allowScroll_)) return false;
	
	std::string scrollUpKey, scrollDownKey, scrollResetKey;
	if (node->getNamedChild("scrollupkey", scrollUpKey, false))
	{
		scrollUpKey_ = Keyboard::instance()->getKey(scrollUpKey.c_str());
	}
	if (node->getNamedChild("scrolldownkey", scrollDownKey, false))
	{
		scrollDownKey_ = Keyboard::instance()->getKey(scrollDownKey.c_str());
	}
	if (node->getNamedChild("scrollresetkey", scrollResetKey, false))
	{
		scrollResetKey_ = Keyboard::instance()->getKey(scrollResetKey.c_str());
	}
	return true;
}
