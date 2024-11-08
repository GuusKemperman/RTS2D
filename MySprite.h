#pragma once
class MySprite
{
public:
	MySprite(shared_ptr<Surface> surface, uint numOfColumns, uint numofRows);
	MySprite(const string& spriteName);
	~MySprite();

	MySprite(const MySprite& other);

	bool DrawScaled(vector<bool>& target, const uint& targetWidth, const uint2& position, const uint& scale) const;
	void DrawScaled(Surface* target, const int2& position, const int& scale) const;

	bool Draw(vector<bool>& target, const uint& targetWidth, const uint2& position) const;
	void Draw(Surface* target, const int2 position) const;

	void SetFrame(uint frameIndex);
	void SetFrame(uint2 frame);

	uint SamplePixel(uint2 atPosition) const;

	inline Surface* GetSurface() { return surface.get(); }
	inline uint2 GetFrameSize() const { return frameSize; }
	inline uint GetFrameWidth() const { return frameSize.x; }
	inline uint GetFrameHeight() const { return frameSize.y; }
	inline uint GetNumberOfFrames() const { return numOfColumns * numOfRows; }
	inline uint GetNumberOfRows() const { return numOfRows; }
	inline uint GetNumberOfColumns() const { return numOfColumns; }
private:
	void Load(shared_ptr<Surface> surface, uint framesRow, uint framesColumn);

	shared_ptr<Surface> surface{};
	uint numOfColumns{};
	uint numOfRows{};
	uint2 frameSize{};

	uint2 currentFrame{};
};