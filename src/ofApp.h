#pragma once
#include <map>

#include "ofMain.h"
#include "ofxImGui.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		//my things
		struct glyphShade
		{
			int shade = 0;
			char asciiDec = 0;
			ofTexture glyph;
			bool operator<(const glyphShade& rhs) const
			{
				return shade < rhs.shade;
			}
			bool operator() (const glyphShade& left, const glyphShade& right)
			{
				return left.shade < right.shade;
			}
			bool operator() (const glyphShade& left, int right)
			{
				return left.shade < right;
			}
			bool operator() (int left, const glyphShade& right)
			{
				return left < right.shade;
			}
			
		};
		struct glyphComparisons
		{
			bool operator() (const glyphShade& left, const glyphShade& right)
			{
				return left.shade < right.shade;
			}
			bool operator() (const glyphShade& left, int right)
			{
				return left.shade < right;
			}
			bool operator() (int left, const glyphShade& right)
			{
				return left < right.shade;
			}
		};
		

		std::vector<glyphShade> glyphShades;

		void grayScaleImage(ofImage& image);
		void pixellate(ofImage& image, size_t pixellationSizeX, size_t pixellationSizeY);
		void asciiGlyphTable(const ofTrueTypeFont font, 
			std::vector<glyphShade>& glyphShades,
			const char startRange, 
			const char endRange);
		void remap(std::vector<glyphShade>& glyphShades, float output_start, float output_end);
		void makeAsciiImage(std::vector<glyphShade>& glyphShades, 
							ofImage& image, 
							size_t pixellationSizeX,
							size_t pixellationSizeY);
		
		int gridDivisionsX = 192;
		int gridDivisionsY = 78;
	
		ofVideoPlayer player;
		ofTexture videoTexture;
		ofImage vidImage;
		std::vector<ofImage> images;
		size_t currImage = 0;

		ofxImGui::Gui gui;
		bool drawImageUnder = true;
		bool drawAscii = false;
		bool isVideo = false;
		bool isGrayScale = false;
		bool isPrePixellated = false;
		bool colorText = false;

		//Sorry MSW only
		std::wstring ExePath();
		std::string ExePathS();

		ofTrueTypeFont font;
		ofImage img;
		
};
