#ifndef GUI2DCOMPONENT_H
#define GUI2DCOMPONENT_H

#include <map>
#include <vector>
#include "Component.h"
class Window;
class Texture;
class Text;
class Font;
struct TextData;

namespace component
{
	class GUI2DComponent : public Component
	{
	public:
		GUI2DComponent(Entity* parent);
		virtual ~GUI2DComponent();

		std::map<std::string, TextData>* const GetTextDataMap();
		TextData* GetTextData(std::string name);

		void AddText(std::string name);
		void SubmitText(Text* text, std::string name);
		void ReplaceText(Text* text, std::string name);

		void SetFont(Font* font);
		void SetText(std::string text, std::string name);
		void SetPos(float2 textPos, std::string name);
		void SetScale(float2 scale, std::string name);
		void SetPadding(float2 padding, std::string name);
		void SetColor(float4 color, std::string name);

		Font* GetFont() const;
		Texture* GetFontTexture() const;
		Text* GetText(std::string name);
		std::map<std::string, Text*>* GetTextMap();
		const int GetNumOfTexts() const;
		const int GetNumOfCharacters(std::string name);

		void Update(double dt);
		void InitScene();

	private:
		friend class Renderer;

		std::map<std::string, TextData> m_TextDataMap = {};
		std::map<std::string, Text*> m_TextMap = {};
		Font* m_pFont = nullptr;

		void uploadTextData(std::string name);
	};
}
#endif
