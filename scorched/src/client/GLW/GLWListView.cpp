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

#include <GLW/GLWListView.h>
#include <GLW/GLWFont.h>
#include <GLW/GLWTranslate.h>
#include <common/Defines.h>
#include <client/ScorchedClient.h>
#include <engine/GameState.h>
#include <stdio.h>

static const float BorderWidth = 20.0f;

REGISTER_CLASS_SOURCE(GLWListView);

unsigned GLWListView::WordEntry::wordRefCount_ = 1;

GLWListView::GLWListView(float x, float y, float w, float h, 
	int maxLen, float textSize, float scrollSpeed) :
	GLWidget(x, y, w, h), 
	scroll_(x + w - 17, y, h - 1, 0, 1), 
	maxLen_(maxLen), textSize_(textSize),
	scrollSpeed_(scrollSpeed),
	handler_(0), currentPosition_(0.0f)
{
	color_ = GLWFont::widgetFontColor;
	scroll_.setMax((int) lines_.size());
	scroll_.setSee((int) (h_ / 12.0f));
	scroll_.setCurrent(scroll_.getMax());
}

GLWListView::~GLWListView()
{
}

void GLWListView::draw()
{
	glBegin(GL_LINE_LOOP);
		drawShadedRoundBox(x_, y_, w_, h_, 6.0f, false);
	glEnd();

	{
		// Stops each font draw from changing state every time
		GLState currentState(GLState::TEXTURE_ON | GLState::BLEND_ON);
		GLFont2d *font = GLWFont::instance()->getGameFont();

		// Clear the currently stored urls
		urls_.clear();

		// Calculate current position
		float posY = y_ + h_ - (textSize_ + 4.0f);
		int pos = (scroll_.getMax() - scroll_.getSee()) - scroll_.getCurrent();

		// Figure out how many characters we are not drawing because
		// they are off the top of the scrolling window
		int charCount = 0;
		for (int i=0; i<pos; i++)
		{
			LineEntry &lineEntry = lines_[i];

			for (int j=0; j<(int) lineEntry.words_.size(); j++)
			{
				WordEntry &wordEntry = lineEntry.words_[j];
				charCount += (int) wordEntry.word_.size();
			}
		}

		// For each line
		bool newWords = false;
		for (int i=pos; i<(int) scroll_.getMax(); i++)
		{
			// Check the line is valid
			if (i >= 0 && i < (int) lines_.size())
			{
				float width = w_ - BorderWidth;
				float widthUsed = 0.0f;

				// For each word
				LineEntry &lineEntry = lines_[i];
				for (int j=0; j<(int) lineEntry.words_.size(); j++)
				{
					WordEntry &wordEntry = lineEntry.words_[j];

					// Check if there is space left to draw this word
					int possibleChars = font->getChars(textSize_, wordEntry.word_.c_str(), width - widthUsed);
					int drawChars = possibleChars;
					if (scrollSpeed_ > 0.0f)
					{
						if (possibleChars + charCount > (int) currentPosition_)
						{
							drawChars = int(currentPosition_) - charCount;
						}
					}

					if (drawChars > 0)
					{
						// Draw this word
						font->drawSubStr(0, drawChars,
							wordEntry.color_,
							textSize_,
							x_ + 5.0f + widthUsed, posY, 0.0f, 
							S3D::formatStringBuffer("%s", wordEntry.word_.c_str()));

						// Send the event (if any)
						if (wordEntry.wordRef_ != wordEntry.wordRefCount_)
						{
							newWords = true;
							wordEntry.wordRef_ = wordEntry.wordRefCount_;
							if (!wordEntry.event_.empty() && handler_)
							{
								handler_->event(wordEntry.event_);
							}
						}

						// Draw the url (if any)
						if (wordEntry.href_.size() > 0)
						{
							drawUrl(wordEntry, drawChars, widthUsed, posY);
						}
					}

					// Add this word to the space used
					float wordWidth = font->getWidth(
						textSize_, wordEntry.word_.c_str());
					widthUsed += wordWidth;
					charCount += (int) wordEntry.word_.size();
				}

				// Move down a line
				posY -= (textSize_ + 2.0f);
			}

			// Check if we should scroll down
			if (posY < y_) 
			{
				if (scrollSpeed_ > 0.0f && newWords)
				{
					scroll_.setCurrent(scroll_.getCurrent() - 1);
				}
				break;
			}

			// Check if we have drawn more than we should see
			if ((scrollSpeed_ > 0.0f) && (charCount > int(currentPosition_))) break;
		}
	}

	scroll_.draw();
}

void GLWListView::simulate(float frameTime)
{
	currentPosition_ += (frameTime * scrollSpeed_);
	scroll_.simulate(frameTime);
}

void GLWListView::mouseDown(int button, float x, float y, bool &skipRest)
{
	std::vector<UrlEntry>::iterator itor;
	for (itor = urls_.begin();
		itor != urls_.end();
		++itor)
	{
		UrlEntry &entry = *itor;
		if (inBox(x, y, entry.x_, entry.y_, entry.w_, entry.h_))
		{
			if (handler_)
			{
				const char *url = entry.entry_->href_.c_str();
				handler_->url(url);
			}
			skipRest = true;
			return;
		}
	}

	scroll_.mouseDown(button, x, y, skipRest);
}

void GLWListView::mouseUp(int button, float x, float y, bool &skipRest)
{
	scroll_.mouseUp(button, x, y, skipRest);
}

void GLWListView::mouseDrag(int button, float mx, float my, float x, float y, bool &skipRest)
{
	scroll_.mouseDrag(button, mx, my, x, y, skipRest);
}

void GLWListView::mouseWheel(float x, float y, float z, bool &skipRest)
{
	if (inBox(x, y, x_, y_, w_, h_))
	{
		skipRest = true;

		if (z < 0.0f) scroll_.setCurrent(scroll_.getCurrent() + 1);
		else scroll_.setCurrent(scroll_.getCurrent() - 1);
	}
}

void GLWListView::clear()
{
	lines_.clear();
	scroll_.setCurrent(0);
	resetPosition();
}

void GLWListView::endPosition()
{
	scroll_.setCurrent(scroll_.getMin());
}

void GLWListView::resetPosition()
{
	GLWListView::WordEntry::wordRefCount_++;
	currentPosition_ = 0.0f;
}

bool GLWListView::addWordEntry(std::list<WordEntry> &words,
	std::string &word, XMLNode *parentNode)
{
	WordEntry wordEntry(word.c_str(), color_);
	word = "";

	if (0 == strcmp("event", parentNode->getName()))
	{
		wordEntry.color_ = Vector(0.4f, 0.0f, 0.0f);

		std::list<XMLNode *> &parameters = parentNode->getParameters();
		std::list<XMLNode *>::iterator itor;
		for (itor = parameters.begin();
			itor != parameters.end();
			++itor)
		{
			XMLNode *node = *itor;
			wordEntry.event_[node->getName()] = node->getContent();
		}
	} 
	else if (0 == strcmp("a", parentNode->getName()))
	{
		wordEntry.color_ = Vector(0.4f, 0.0f, 0.0f);
		if (!parentNode->getNamedParameter("href", wordEntry.href_, true, false))
		{
			return false;
		}
	}

	words.push_back(wordEntry);
	return true;
}

bool GLWListView::getLines(std::list<WordEntry> &words, float &lineLen)
{
	std::list<WordEntry>::iterator itor;
	for (itor = words.begin();
		itor != words.end();
		++itor)
	{
		WordEntry &wordEntry = *itor;

		if (wordEntry.word_.c_str()[0] == '\n')
		{
			// Add a new line
			LineEntry lineEntry;
			lines_.push_back(lineEntry);
			lineLen = 0.0f;
		}
		else
		{
			// Check if weve run out of space on the current line
			float wordLen = 
				GLWFont::instance()->getGameFont()->
				getWidth(textSize_, wordEntry.word_.c_str());
			if (wordLen + lineLen >= w_ - BorderWidth)
			{
				// Add a new line
				LineEntry lineEntry;
				lines_.push_back(lineEntry);
				lineLen = 0.0f;
			}
		
			lines_.back().words_.push_back(wordEntry);
			lineLen += wordLen;
		}
	}

	return true;
}

bool GLWListView::getWords(XMLNode *node, std::list<WordEntry> &words)
{
	// For each child XML node
	std::list<XMLNode *>::iterator childrenItor;
	std::list<XMLNode *> children = node->getChildren();
	for (childrenItor = children.begin();
		childrenItor != children.end();
		++childrenItor)
	{
		XMLNode *child = (*childrenItor);

		// Get the child node type
		if (child->getType() == XMLNode::XMLNodeType)
		{
			// Its another node, recurse over its children too
			if (!getWords(child, words)) return false;
		}
		else
		{
			// Its a text type, add the words from the text
			std::string word;
			for (const char *t=child->getContent(); *t; t++)
			{
				if (*t == '\n')
				{
					// Add the current word
					if (!addWordEntry(words, word, node)) return false;

					word = "\n";
					if (!addWordEntry(words, word, node)) return false;
				}
				else
				{
					word += *t;

					// A word break
					if (*t == ' ')
					{
						// Add a new word
						if (!addWordEntry(words, word, node)) return false;
					}
				}
			}

			// Add any words we've got left over
			if (!addWordEntry(words, word, node)) return false;
		}
	}

	return true;
}

bool GLWListView::addXML(XMLNode *node)
{
	// Recurse over the document adding the words
	std::list<WordEntry> words;
	if (!getWords(node, words)) return false;

	// Add a blank line to start with
	LineEntry lineEntry;
	lines_.push_back(lineEntry);
	float lineLen = 0.0f;
	getLines(words, lineLen);

	// Setup the current scroll position
	setScroll();

	return true;
}

void GLWListView::addLine(const std::string &text)
{
	// Remove extra lines
	if (maxLen_ > 0)
	{
		if (lines_.size() > (unsigned int) maxLen_) lines_.clear();
	}

	// Generate the line to add (add a single word)
	WordEntry wordEntry(text.c_str(), color_);
	LineEntry lineEntry;
	lineEntry.words_.push_back(wordEntry);
	lines_.push_back(lineEntry);

	// Setup the current scroll position
	setScroll();
}

void GLWListView::setScroll()
{
	int view = (int) (h_ / (textSize_ + 2.0f));
	scroll_.setMax((int) lines_.size());
	scroll_.setSee(view);
	scroll_.setCurrent(scroll_.getMax() - scroll_.getSee());
}

void GLWListView::drawUrl(WordEntry &wordEntry, int drawChars, float x, float y)
{
	GLFont2d *font = GLWFont::instance()->getGameFont();
	float partWordWidth = font->getWidth(
		textSize_, wordEntry.word_.c_str(), drawChars);

	// Add the new url entry
	UrlEntry urlEntry;
	urlEntry.x_ = x_ + 5.0f + x;
	urlEntry.y_ = y - 2.0f;
	urlEntry.w_ = partWordWidth;
	urlEntry.h_ = textSize_ + 2.0f;
	urlEntry.entry_ = &wordEntry;
	urls_.push_back(urlEntry);

	// Draw the underline
	GLState noTexState(GLState::TEXTURE_OFF);
	glLineWidth(2.0f);
	glColor3fv(wordEntry.color_);
	glBegin(GL_LINES);
		glVertex2f(x_ + 5.0f + x, y - 2.0f);
		glVertex2f(x_ + 5.0f + x + partWordWidth, y - 2.0f);
	glEnd();
	glLineWidth(1.0f);
}
