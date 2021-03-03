#include "ofApp.h"
#include <string>

void ofApp::grayScaleImage(ofImage& image)
{
	for (size_t y = 0; y < image.getHeight(); y++)
	{
		for (size_t x = 0; x < image.getWidth(); x++)
		{
			ofColor newColor;
			ofColor oldColor = image.getColor(x, y);
			unsigned char grayColor = (oldColor.r + oldColor.g + oldColor.b) / 3;
			newColor.set(grayColor, grayColor, grayColor,255);
			image.setColor(x, y, newColor);
		}
	}
	image.update();
}
void ofApp::pixellate(ofImage& image, size_t pixellationSizeX, size_t pixellationSizeY)
{
	size_t width = image.getWidth();
	size_t height = image.getHeight();
	ofPixels pix = image.getPixels();
	size_t pixStrideX = image.getWidth() / pixellationSizeX;
	size_t pixStrideY = image.getHeight() / pixellationSizeY;
	size_t subAreaSize = pixStrideX * pixStrideY;
	ofImage img;
	
	//pre allocate vector to recieve color samples.
	
	std::vector<ofColor> sampledColors;
	sampledColors.reserve(pixStrideX*pixStrideY);

	//loop through large regions
	for (size_t y = 0; y < image.getHeight(); y += pixStrideY)
	{
		for (size_t x = 0; x < image.getWidth(); x += pixStrideX)
		{
			//loop through small regions gather pixel data
			size_t pixelsCounted = 0;
			
			//make sure to clear vector each sub loop
			sampledColors.clear();
			for (size_t i = y; i < y + pixStrideY; i++)
			{
				
				for (size_t j = x; j < x + pixStrideX; j++)
				{
					//sample each pixel in this sub region;
					if (j >= 0 && j < image.getWidth() &&
						i >= 0 && i < image.getHeight())
					{
						pixelsCounted++;
						sampledColors.push_back(image.getColor(j, i));
						//sampledCols[i + j] = image.getColor(j, i);
					}
				}
			}
			//Add up the colors and divide by count to get average.
			size_t sumColorR = 0;
			size_t sumColorG = 0;
			size_t sumColorB = 0;

			for (auto& c : sampledColors)
			{
				sumColorR += c.r;
				sumColorG += c.g;
				sumColorB += c.b;
			}
			if (sampledColors.size() != 0)
			{
				sumColorR /= sampledColors.size();
				sumColorG /= sampledColors.size();
				sumColorB /= sampledColors.size();
			}
			
			ofColor newColor;
			newColor.set(sumColorR, sumColorG, sumColorB);
			//now set the color of that subregion.
			for (size_t i = y; i < y + pixStrideY; i++)
			{
				for (size_t j = x; j < x + pixStrideX; j++)
				{
					if (j >= 0 && j < image.getWidth() &&
						i >= 0 && i < image.getHeight())
					{
						image.setColor(j, i, newColor);
					}
				}
			}
		}
	}
	
	image.update();
}

void ofApp::asciiGlyphTable(const ofTrueTypeFont font,
	std::vector<glyphShade>& glyphShades,
	const char startRange,
	const char endRange)
{
	for (char i = startRange; i <= endRange; i++)
	{
		glyphShade gs;
		gs.asciiDec = i;
		std::string character;
		character.push_back(i);
		gs.glyph = font.getStringTexture(character);
		ofFbo fb;
		fb.allocate(gs.glyph.getWidth(), gs.glyph.getHeight(), GL_RGB);
		fb.begin();
		ofClear(0, 0, 0);
		gs.glyph.draw(0,0);
		fb.end();
		ofPixels pixels;
		fb.readToPixels(pixels);
		size_t numpix = pixels.getWidth() * pixels.getHeight();
		float sumpix = 0;
		for (size_t y = 0; y < pixels.getHeight(); y++)
		{
			for (size_t x = 0; x < pixels.getWidth(); x++)
			{
				sumpix += pixels.getColor(x,y).getBrightness();
			}
		}
		float total = float(sumpix / numpix);
		//TEST
		fb.draw((i-startRange) * 32,0);
		gs.shade = total;
		glyphShades.push_back(gs);
	}
}

void ofApp::remap(std::vector<glyphShade>& glyphShades, float output_start, float output_end)
{
	double slope = 1.0 * (output_end - output_start) / (glyphShades.back().shade - glyphShades.front().shade);
	auto input_start = glyphShades.front().shade;
	for (auto& shade : glyphShades)
	{
		shade.shade = output_start + slope * (shade.shade - input_start);
	}
	
}

void ofApp::makeAsciiImage(std::vector<glyphShade>& glyphShades, 
							ofImage& image, 
							size_t pixellationSizeX,
							size_t pixellationSizeY)
{
	size_t pixStrideX = image.getWidth() / pixellationSizeX;
	size_t pixStrideY = image.getHeight() / pixellationSizeY;

	for (size_t y = 0; y < image.getHeight(); y += pixStrideY)
	{
		for (size_t x = 0; x < image.getWidth(); x += pixStrideX)
		{
			ofRectangle rect = { float(x), float(y), float(pixStrideX), float(pixStrideY) };
			auto it = std::lower_bound(glyphShades.begin(), glyphShades.end(), image.getColor(x,y).getBrightness(), glyphShade());
			int index = std::distance(glyphShades.begin(), it);
			if (index >= 0 && index < glyphShades.size())
			{
				if (colorText)
				{
					ofSetColor(image.getColor(x, y));
				}
				
				glyphShades[index].glyph.draw(rect);
			}
		}
	}
}

std::wstring ofApp::ExePath()
{
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}

std::string ofApp::ExePathS()
{
	CHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::string(buffer).find_last_of("\\/");
	return std::string(buffer).substr(0, pos);
}

//--------------------------------------------------------------
void ofApp::setup(){
	//filepath
	std::wstring path = ExePath();
	std::string pathS = ExePathS();

	//GUI
	gui.setup();
	
	//Video
	player.load(pathS + "\\SanFran.mov");
	player.play();
	videoTexture.allocate(player.getWidth(), player.getHeight(), OF_PIXELS_RGBA);

	//Image
	
	images.push_back(ofImage(path + L"\\/statue.jpg"));
	images.push_back(ofImage(path + L"\\/colorFace.jpg"));
	images.push_back(ofImage(path + L"\\/face.jpg"));
	images.push_back(ofImage(path + L"\\/trees.jpg"));
	images.push_back(ofImage(path + L"\\/Cat.jpg"));
	images.push_back(ofImage(path + L"\\/daisy.jpg"));

	//Fonts
	font.load(path + L"\\/consola.ttf", 32);
	asciiGlyphTable(font, glyphShades, 32, 126);
	std::sort(glyphShades.begin(), glyphShades.end());
	//Now Remap to Char vals;
	remap(glyphShades, 0.0f, 255.0f);
}

//--------------------------------------------------------------
void ofApp::update(){
	if (isVideo)
	{
		player.update();
		if (player.isFrameNew())
		{
			vidImage.setFromPixels(player.getPixels());
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(0);
	ofSetColor(255);
	if (isVideo)
	{
		
		if (isGrayScale)
			grayScaleImage(vidImage);
		if (isPrePixellated)
			pixellate(vidImage, gridDivisionsX, gridDivisionsY);
		if (drawImageUnder)
			vidImage.draw(0, 0);
		if (drawAscii)
			makeAsciiImage(glyphShades, vidImage, gridDivisionsX, gridDivisionsY);
	}
	else 
	{
		if (images[currImage].isAllocated())
		{
			if (isGrayScale)
				grayScaleImage(images[currImage]);
			if (isPrePixellated)
				pixellate(images[currImage], gridDivisionsX, gridDivisionsY);
			if (drawImageUnder)
				images[currImage].draw({ 0.0f, 0.0f, 0.0f });
			if (drawAscii)
				makeAsciiImage(glyphShades, images[currImage], gridDivisionsX, gridDivisionsY);
		}
	}
	
	//displayframerate
	
	std::stringstream ss; 
	ss << "Frame Rate : " << ofGetFrameRate() << "\nCurrent Image No : " << currImage; 
	font.drawString(ss.str(), 10, 1150);
	
	//Gui stuff
	gui.begin();
	ImGui::InputInt("gridDivisionsX : ", &gridDivisionsX);
	ImGui::InputInt("gridDivisionsY : ", &gridDivisionsY);
	if (ImGui::Button("Show Video"))
	{
		isVideo = true;
	}
	if (ImGui::Button("Show Image"))
	{
		isVideo = false;
	}
	if (ImGui::Button("Next Image"))
	{
		currImage++;
		if (currImage >= images.size())
		{
			currImage = 0;
		}
	
	}
	if (ImGui::Button("GrayScale Image"))
	{
		isGrayScale = true;
	}
	if (ImGui::Button("Colour Image"))
	{
		isGrayScale = false;
	}
	if (ImGui::Button("Pre-Pixellate Image"))
	{
		isPrePixellated = true;
	}
	if (ImGui::Button("No Advance Pixellation"))
	{
		isPrePixellated = false;
	}
	if (ImGui::Button("Draw Image"))
	{
		drawImageUnder = true;
	}
	if (ImGui::Button("Don't Draw Image"))
	{
		drawImageUnder = false;
	}
	if (ImGui::Button("Draw ASCII"))
	{
		drawAscii = true;
	}
	if (ImGui::Button("Don't Draw ASCII"))
	{
		drawAscii = false;
	}
	if (ImGui::Button("Colour the text"))
	{
		colorText = true;
	}
	if (ImGui::Button("White Text"))
	{
		colorText = false;
	}
	gui.draw();
	gui.end();
}
