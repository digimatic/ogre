/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2015 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#pragma once

#include "Ogre.h"		 
#include "OgreHlmsPrerequisites.h"
#include "OgreHlmsPropertyMap.h"
#include "OgreHlmsShaderCommon.h"
#include "OgreIdString.h"

namespace Ogre
{
	/** \addtogroup Component
	*  @{
	*/
	/** \addtogroup Hlms
	*  @{
	*/
	class _OgreHlmsExport ShaderGenerator
	{
	protected:

		typedef std::map<IdString, Ogre::String> PiecesMap;

		enum ExpressionType
		{
			EXPR_OPERATOR_OR,        //||
			EXPR_OPERATOR_AND,       //&&
			EXPR_OBJECT,             //(...)
			EXPR_VAR
		};

		struct Expression
		{
			bool                    result;
			bool                    negated;
			ExpressionType          type;
			std::vector<Expression> children;
			Ogre::String            value;

			Expression() : result(false), negated(false), type(EXPR_VAR) {}
		};

		typedef std::vector<Expression> ExpressionVec;

		static void copy(Ogre::String &outBuffer, const SubStringRef &inSubString, size_t length);
		static void repeat(Ogre::String &outBuffer, const SubStringRef &inSubString, size_t length,
			size_t passNum, const Ogre::String &counterVar);
		static bool parseMath(const Ogre::String &inBuffer, Ogre::String &outBuffer, PropertyMap &properties);
		static bool parseForEach(const Ogre::String &inBuffer, Ogre::String &outBuffer, PropertyMap &properties);
		static bool parseProperties(Ogre::String &inBuffer, Ogre::String &outBuffer, PropertyMap &properties);
		static bool collectPieces(const Ogre::String &inBuffer, Ogre::String &outBuffer, PropertyMap &properties, PiecesMap& pieces);
		static bool insertPieces(Ogre::String &inBuffer, Ogre::String &outBuffer, PropertyMap &properties, PiecesMap& pieces);
		static bool parseCounter(const Ogre::String &inBuffer, Ogre::String &outBuffer, PropertyMap &properties);

		/** Goes through 'buffer', starting from startPos (inclusive) looking for the given
			character while skipping whitespace. If any character other than whitespace or
			EOLs if found returns String::npos
			@return
			String::npos if not found or wasn't the next character. If found, the position
			in the buffer, from start
			*/
		static size_t findNextCharacter(const Ogre::String &buffer, size_t startPos, char character);

		static void findBlockEnd(SubStringRef &outSubString, bool &syntaxError);

		static bool evaluateExpression(SubStringRef &outSubString, bool &outSyntaxError, PropertyMap &properties);
		static bool evaluateExpressionRecursive(ExpressionVec &expression, bool &outSyntaxError, PropertyMap &properties);
		static size_t evaluateExpressionEnd(const SubStringRef &outSubString);

		static void evaluateParamArgs(SubStringRef &outSubString, Ogre::StringVector &outArgs,
			bool &outSyntaxError);

		static size_t calculateLineCount(const Ogre::String &buffer, size_t idx);
		static size_t calculateLineCount(const SubStringRef &subString);

	public:
		static Ogre::String parse(Ogre::String &inBuffer, PropertyMap &properties, Ogre::StringVector& pieceFiles);
	};
}
