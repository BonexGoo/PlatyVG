#include <platyvg.hpp>
#include "export.hpp"
#include "../core/view.hpp"

enum {BytesPerPixel = 3, BitsWidth = 512, BitsHeight = 512};
struct BitmapFile {uint32 size; sint16 param1; sint16 param2; uint32 offbits;};
struct BitmapInfo {uint32 size; sint32 width; sint32 height; uint16 planes; uint16 bitcount; uint32 compression;
	uint32 size_image; sint32 xpels_meter; sint32 ypels_meter; uint32 color_used; uint32 color_important;};

static void WriteChunkString(uint08*& bits, chars string, const sint08 endcode)
{
	// String
	while(*string)
	{
		*(bits++) = ((*string & 0xF0) >> 1) | 0x84;
		*(bits++) = ((*string & 0x0F) << 3) | 0x84;
		++string;
	}

	// EndCode
	*(bits++) = ((endcode & 0xF0) >> 1) | 0x84;
	*(bits++) = ((endcode & 0x0F) << 3) | 0x84;
}

static void WriteChunkNumber(uint08*& bits, sint32 number, const sint08 endcode)
{
	// Mark
	const sint08 Mark = (number < 0)? '-' : '+';
	*(bits++) = ((Mark & 0xF0) >> 1) | 0x84;
	*(bits++) = ((Mark & 0x0F) << 3) | 0x84;

	// Number
	if(number < 0) number = -number;
	do
	{
		const sint08 Number = '0' + (number % 10);
		*(bits++) = ((Number & 0xF0) >> 1) | 0x84;
		*(bits++) = ((Number & 0x0F) << 3) | 0x84;
	}
	while(number /= 10);

	// EndCode
	*(bits++) = ((endcode & 0xF0) >> 1) | 0x84;
	*(bits++) = ((endcode & 0x0F) << 3) | 0x84;
}

static chars DecodeChunk(uint08*& bits)
{
	sint08* Focus = (sint08*) bits;
	chars Result = Focus;
	for(sint08 OneChar = '\0'; OneChar != '>';)
	{
		OneChar = (*(bits++) & 0x78) << 1;
		OneChar |= (*(bits++) & 0x78) >> 3;
		*(Focus++) = OneChar;
	}
	*Focus = '\0';
	return Result;
}

static const sint32 ParseNumber(chars chunk, chars* nextchunk = nullptr)
{
	sint32 Result = 0, Level = 1;
	bool IsMinus = (*(chunk++) == '-');
	while('0' <= *chunk && *chunk <= '9')
	{
		Result += (*(chunk++) - '0') * Level;
		Level *= 10;
	}
	if(nextchunk)
		*nextchunk = chunk + 1;
	return (IsMinus)? -Result : Result;
}

static bool IsChunk(chars name, const sint08 (&cmp)[5])
{
	return name[0] == cmp[0]
		&& name[1] == cmp[1]
		&& name[2] == cmp[2]
		&& name[3] == cmp[3];
}

class N1D7
{
	enum {BitNum = 7, BitFlag = 0x7F};

public:
	static void WriteBinaries(uint08*& bits, bytes binaries, const sint32 size)
	{		
		sint32 ByteFocus = 0, BitFocus = 0;
		while(ByteFocus < size)
		{
			uint08 OneByte = binaries[ByteFocus] >> BitFocus;
			if(0 < BitFocus && ++ByteFocus < size)
				OneByte |= binaries[ByteFocus] << (8 - BitFocus);
			*(bits++) = 0x80 | OneByte;
			BitFocus = (BitFocus + BitNum) & (8 - 1);
		}
	}

	static bytes DecodeBinaries(uint08*& bits, const sint32 size)
	{
		uint08* Result = bits;
		sint32 ByteFocus = 0, BitFocus = 0;
		while(ByteFocus < size)
		{
			const uint08 OneByte = *(bits++) & BitFlag;
			if(BitFocus == 0) Result[ByteFocus] = OneByte;
			else Result[ByteFocus++] |= OneByte << BitFocus;
			if(8 - BitNum < BitFocus && ByteFocus < size)
				Result[ByteFocus] = OneByte >> (8 - BitFocus);
			BitFocus = (BitFocus + BitNum) & (8 - 1);
		}
		return Result;
	}

	static const sint32 CalcLength(const sint32 size)
	{
		return (BitNum * size + 8 - 1) / 8;
	}
};

namespace PlatyVGApi
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_flag, Export_Load, api_int doc, api_string filename, api_flag usingArea, api_flag usingZoom)
    PX_API_IMPL(return Export::Load(doc, PX_API_STR(filename), usingArea, usingZoom))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Export::Load(const sint32 doc, chars filename, const bool usingArea, const bool usingZoom)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		PX_ASSERT("해당 자료가 없습니다", document);
		if(!document) return false;

		// ■ Load bitmap
		const void* FileR = File::OpenForRead(filename);
		if(FileR == nullptr)
		{
			PX_ASSERT("파일이 존재하지 않습니다", false);
			return false;
		}
		const sint32 BmpSize = File::GetSize(FileR) - 2;
		uint08* Bmp = (uint08*) Buffer::Alloc(NEEDS BmpSize, true);
		File::Skip(FileR, 2);
		File::Read(FileR, Bmp, BmpSize);
		File::Close(FileR);

		// ■ Bitmap bits
		uint08* Bits = (uint08*) (Bmp + sizeof(BitmapFile) + sizeof(BitmapInfo));
		bytes BitsEnd = Bmp + BmpSize;
		chars FileHeader = DecodeChunk(Bits); // ▶
		PX_ASSERT("알 수 없는 파일입니다", CONST_STRING("<PenGenerations Viewable Vector Graphics Format>") == FileHeader);

		bool NeedMore = false, NeedUpdate = false;
		while(Bits < BitsEnd)
		{
			chars ChunkHeader = DecodeChunk(Bits); // ▶
			if(IsChunk(ChunkHeader, "<UUI")) // UUID
			{
				////////////////////
			}
			else if(IsChunk(ChunkHeader, "<ARE")) // AREA
			{
				if(!usingArea) continue;
				const sint32 Number1 = ParseNumber(ChunkHeader += 6, &ChunkHeader);
				const sint32 Number2 = ParseNumber(ChunkHeader, &ChunkHeader);
				const sint32 Number3 = ParseNumber(ChunkHeader, &ChunkHeader);
				const sint32 Number4 = ParseNumber(ChunkHeader);
				const float L = *((float*) &Number1);
				const float T = *((float*) &Number2);
				const float R = *((float*) &Number3);
				const float B = *((float*) &Number4);
				if(document->Area.l != L || document->Area.t != T || document->Area.r != R || document->Area.b != B)
				{
					document->Area.l = L;
					document->Area.t = T;
					document->Area.r = R;
					document->Area.b = B;
					NeedUpdate = true;
				}
			}
			else if(IsChunk(ChunkHeader, "<ZOO")) // ZOOM
			{
				if(!usingZoom) continue;
				const sint32 Number1 = ParseNumber(ChunkHeader += 6, &ChunkHeader);
				const sint32 Number2 = ParseNumber(ChunkHeader, &ChunkHeader);
				const sint32 Number3 = ParseNumber(ChunkHeader);
				const float X = *((float*) &Number1);
				const float Y = *((float*) &Number2);
				const float S = *((float*) &Number3);
				if(document->Transform.offset.x != X || document->Transform.offset.y != Y || document->Transform.scale != S)
				{
					document->Transform.offset.x = X;
					document->Transform.offset.y = Y;
					document->Transform.scale = S;
					NeedUpdate = true;
				}
			}
			else if(IsChunk(ChunkHeader, "<SPL") || IsChunk(ChunkHeader, "<TUB") || IsChunk(ChunkHeader, "<AQU")) // SPLINE, TUBE, AQUA
			{
				const bool IsSpline = IsChunk(ChunkHeader, "<SPL");
				const bool IsTube = IsChunk(ChunkHeader, "<TUB");
				if(IsChunk(ChunkHeader += (IsSpline)? 8 : 6, "N1D7"))
				{
					static const sint32 EdgeSplineSize = EdgeSpline().LengthOfWrite();
					static const sint32 EdgeTubeSize = EdgeTube().LengthOfWrite();
					const sint32 EdgeSize = (IsSpline)? EdgeSplineSize : EdgeTubeSize;
					const sint32 LayerIndex = ParseNumber(ChunkHeader += 5, &ChunkHeader);
					const sint32 EdgeCount = ParseNumber(ChunkHeader, &ChunkHeader);
					const sint32 NumberC = ParseNumber(ChunkHeader, &ChunkHeader);
					const sint32 NumberA = ParseNumber(ChunkHeader, &ChunkHeader);
					const sint32 NumberT = ParseNumber(ChunkHeader);
					Context NewContext(Color(*((uint32*) &NumberC), *((uint32*) &NumberA)), *((float*) &NumberT));

					Shape* NewShape = nullptr;
					if(IsSpline)
					{
						EdgeSplines EdgeLoader;
						for(sint32 e = 0; e < EdgeCount; ++e)
						{
							bytes Binaries = N1D7::DecodeBinaries(Bits, EdgeSize); // ▶
							EdgeLoader.AtAdding() = EdgeSpline(Binaries);
						}
						ShapeSpline* NewShapeSpline = (ShapeSpline*) Buffer::Alloc<ShapeSpline>(NEEDS 1);
						NewShapeSpline->Init(NewContext, EdgeLoader);
						NewShape = NewShapeSpline;
					}
					else
					{
						EdgeTubes EdgeLoader;
						for(sint32 e = 0; e < EdgeCount; ++e)
						{
							bytes Binaries = N1D7::DecodeBinaries(Bits, EdgeSize); // ▶
							EdgeLoader.AtAdding() = EdgeTube(Binaries);
						}
						ShapeTube* NewShapeTube = (ShapeTube*) ((IsTube)?
							Buffer::Alloc<ShapeTube>(NEEDS 1) : Buffer::Alloc<ShapeTubeAqua>(NEEDS 1));
						NewShapeTube->Init(NewContext, EdgeLoader);
						NewShape = NewShapeTube;
					}

					if(!NeedUpdate)
						NewShape->Update(document->Transform.offset, document->Transform.scale, view->OptionRadius);
					document->AllLayers.AtWherever(LayerIndex).AllShapes.AtAdding() = (buffer) NewShape;
				}
				else PX_ASSERT("알 수 없는 인코딩입니다", false);
			}
			else if(IsChunk(ChunkHeader, "<CRC")) // CRC
			{
				////////////////////
			}
			else if(IsChunk(ChunkHeader, "<CON")) // CONTINUE
			{
				NeedMore = true;
				break;
			}
			else if(IsChunk(ChunkHeader, "<END")) // END
				break;
			else PX_ASSERT("알 수 없는 청크입니다", false);
		}
		PX_ASSERT("파일의 길이가 부족합니다", Bits <= BitsEnd);

		if(NeedUpdate)
			document->UpdateLayers(view->OptionRadius);
		Buffer::Free((buffer) Bmp);
		return NeedMore;
	}

	bool Export::LoadShape(const sint32 doc, const sint32 layer, bytes shapebuffer, sint32 buffersize)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		PX_ASSERT("해당 자료가 없습니다", document);
		if(!document) return false;

		if(CONST_STRING("PlatyVGElement::EdgeTube") == (chars) shapebuffer)
		{
			shapebuffer += 24 + 1;
			buffersize -= 24 + 1;
			static const sint32 EdgeTubeSize = EdgeTube().LengthOfWrite();
			ShapeTubeAqua* NewShapeTubeAqua = (ShapeTubeAqua*) Buffer::Alloc<ShapeTubeAqua>(NEEDS 1);
			// Context
			Context NewContext;
			Memory::Copy(&NewContext.color.rgba, shapebuffer + 0, 4);
			Memory::Copy(&NewContext.color.aqua, shapebuffer + 4, 4);
			Memory::Copy(&NewContext.thick, shapebuffer + 8, 4);
			NewShapeTubeAqua->SetContext(NewContext);
			shapebuffer += 12;
			buffersize -= 12;
			// Edges
			while(EdgeTubeSize <= buffersize)
			{
				NewShapeTubeAqua->AddEdge(shapebuffer);
				shapebuffer += EdgeTubeSize;
				buffersize -= EdgeTubeSize;
			}
			// Update
			NewShapeTubeAqua->Update(document->Transform.offset, document->Transform.scale, view->OptionRadius);
			document->AllLayers.AtWherever(layer).AllShapes.AtAdding() = (buffer) NewShapeTubeAqua;
			return true;
		}
		return false;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_flag, Export_LoadVG, api_int doc, api_int layer, api_string filename)
    PX_API_IMPL(return Export::LoadVG(doc, layer, PX_API_STR(filename)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Export::LoadVG(const sint32 doc, const sint32 layer, chars filename)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		PX_ASSERT("해당 자료가 없습니다", document);
		if(!document) return false;

		// ■ Load vg
		String VgData;
		const void* VGFile = File::OpenForRead(filename);
		if(VGFile == nullptr)
		{
			PX_ASSERT("해당 파일이 존재하지 않습니다", false);
			return false;
		}
		else
		{
			const sint32 VGSize = File::GetSize(VGFile);
			uint08* VG = (uint08*) Buffer::Alloc(NEEDS VGSize + 1, true);
			File::Read(VGFile, VG, VGSize);
			VG[VGSize] = '\0';
			VgData = (chars) VG;
			Buffer::Free((buffer) VG);
			File::Close(VGFile);
		}

		// ■ Parse vg
		Parser VgParser = Parser(VgData);
		if(!VgParser.SeekOf(Parser::Tag("\"pageExtent\":["))) return false;
		const floats PageExtent = VgParser.ReadFloats(',', ']');
		const float PageX = PageExtent[0];
		const float PageY = PageExtent[1];
		const float PageMX = (PageExtent[2] - PageX) / 2;
		const float PageMY = (PageExtent[3] - PageY) / 2;

		Parser::Tag TagType = Parser::Tag("{\"type\":");
		Parser::Tag TagLineWidth = Parser::Tag("\"lineWidth\":");
		Parser::Tag TagLineColor = Parser::Tag("\"lineColor\":\"");
		Parser::Tag TagCount = Parser::Tag("\"count\":");
		Parser::Tag TagPoints = Parser::Tag("\"points\":[");
		Parser::Tag TagForce = Parser::Tag("\"force\":[");
		while(VgParser.SeekOf(TagType))
		{
			const sint32 Type = VgParser.ReadInt();
			if(Type == 33)
			{
				if(!VgParser.SeekOf(TagLineWidth)) return false;
				const float LineWidth = VgParser.ReadFloat();
				if(!VgParser.SeekOf(TagLineColor)) return false;
				const uint32 LineColor = VgParser.ReadHex32();
				if(!VgParser.SeekOf(TagCount)) return false;
				const sint32 Count = VgParser.ReadInt();
				if(!VgParser.SeekOf(TagPoints)) return false;
				const floats Points = VgParser.ReadFloats(',', ']');
				if(!VgParser.SeekOf(TagForce)) return false;
				const sint32s Forces = VgParser.ReadInts(',', ']');
				if(Points.Count() != Count * 2 || Forces.Count() != Count)
					return false;

				Coords Collector;
				for(sint32 i = 0; i < Count; ++i)
					Collector.AtAdding() = Coord(
						(Points[i * 2 + 0] - PageX - PageMX) * 10,
						(PageMY - Points[i * 2 + 1] + PageY) * 10, Forces[i] / 1024.0f);

				ShapeTubeAqua* NewShapeTubeAqua = (ShapeTubeAqua*) Buffer::Alloc<ShapeTubeAqua>(NEEDS 1);
				NewShapeTubeAqua->Process(Context(Color(LineColor, 0), LineWidth * 10), Collector);
				NewShapeTubeAqua->Update(document->Transform.offset, document->Transform.scale, view->OptionRadius);
				document->AllLayers.AtWherever(layer).AllShapes.AtAdding() = (buffer) NewShapeTubeAqua;
			}
		}
		return true;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_int, Export_Save, api_int doc, api_string filename, api_int shapebegin)
    PX_API_IMPL(return Export::Save(doc, PX_API_STR(filename), shapebegin))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	sint32 Export::Save(const sint32 doc, chars filename, const sint32 shapebegin)
	{
		View* view = View::Currently();
		Document* document = view->GetDocument(doc);
		PX_ASSERT("해당 자료가 없습니다", document);
		if(!document) return false;

		// ■ Bitmap header
		const sint32 BmpRow = (BytesPerPixel * BitsWidth + 3) & ~3;
		const sint32 BmpSize = sizeof(BitmapFile) + sizeof(BitmapInfo) + BmpRow * BitsHeight;
		uint08* Bmp = (uint08*) Buffer::Alloc(NEEDS BmpSize, true);
		BitmapFile* BmpFileHeader = (BitmapFile*) Bmp;
		BmpFileHeader->size = 54 + BmpRow * BitsHeight;
		BmpFileHeader->param1 = 0;
		BmpFileHeader->param2 = 0;
		BmpFileHeader->offbits = 54;
		BitmapInfo* BmpInfoHeader = (BitmapInfo*) (Bmp + sizeof(BitmapFile));
		BmpInfoHeader->size = 40;
		BmpInfoHeader->width = BitsWidth;
		BmpInfoHeader->height = BitsHeight;
		BmpInfoHeader->planes = 1;
		BmpInfoHeader->bitcount = 8 * BytesPerPixel;
		BmpInfoHeader->compression = 0;
		BmpInfoHeader->size_image = BmpRow * BitsHeight;
		BmpInfoHeader->xpels_meter = 3780;
		BmpInfoHeader->ypels_meter = 3780;
		BmpInfoHeader->color_used = 0;
		BmpInfoHeader->color_important = 0;

		// ■ Bitmap bits
		uint08* Bits = (uint08*) (Bmp + sizeof(BitmapFile) + sizeof(BitmapInfo));
		bytes BitsBegin = Bits;
		bytes BitsEnd = Bits + BmpRow * BitsHeight;

		// Header
		WriteChunkString(Bits, "<PenGenerations Viewable Vector Graphics Format", '>'); // ◀

		// Uuid
		chars Uuid = "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"; ////////////////////
		WriteChunkString(Bits, "<UUID", '/'); // ◀
		WriteChunkString(Bits, Uuid, '>'); // ◀

		// Area
		WriteChunkString(Bits, "<AREA", '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Area.l), '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Area.t), '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Area.r), '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Area.b), '>'); // ◀

		// Zoom
		WriteChunkString(Bits, "<ZOOM", '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Transform.offset.x), '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Transform.offset.y), '/'); // ◀
		WriteChunkNumber(Bits, *((sint32*) &document->Transform.scale), '>'); // ◀

		// Shapes
		buffer TempBuffer = Buffer::Alloc(NEEDS 0, true);
		sint32 ShapeFocus = 0;
		bool HasBreak = false;
		for(sint32 l = 0, lend = document->AllLayers.Count(); l < lend; ++l)
		for(sint32 i = 0, iend = document->AllLayers[l].AllShapes.Count(); i < iend; ++i, ++ShapeFocus)
		{
			if(ShapeFocus < shapebegin) continue;
			const Shape* OneShape = document->AllLayers[l].AllShapes[i].ptr_const();
			const sint32 EdgeCount = OneShape->GetEdgeCount();
			if(0 < EdgeCount)
			{
				const sint32 EdgeSize = OneShape->GetEdge(0)->LengthOfWrite();
				if(BitsEnd < Bits + 200 + N1D7::CalcLength(EdgeSize) * EdgeCount)
				{
					// 저장규모를 초과한 단일도형은 저장하지 않음
					if(shapebegin < ShapeFocus)
					{HasBreak = true; break;}
					else continue;
				}

				chars MeshType = OneShape->GetMeshType();
				if(CONST_STRING("PlatyVGElement::MeshSpline") == MeshType)
					WriteChunkString(Bits, "<SPLINE", '/'); // ◀
				else if(CONST_STRING("PlatyVGElement::MeshTube") == MeshType)
					WriteChunkString(Bits, "<TUBE", '/'); // ◀
				else if(CONST_STRING("PlatyVGElement::MeshAqua") == MeshType)
					WriteChunkString(Bits, "<AQUA", '/'); // ◀
				else continue;

				const Context OneContext = OneShape->GetContext();
				WriteChunkString(Bits, "N1D7", '/'); // ◀
				WriteChunkNumber(Bits, l, '/'); // ◀
				WriteChunkNumber(Bits, EdgeCount, '/'); // ◀
				WriteChunkNumber(Bits, *((sint32*) &OneContext.color.rgba), '/'); // ◀
				WriteChunkNumber(Bits, *((sint32*) &OneContext.color.aqua), '/'); // ◀
				WriteChunkNumber(Bits, *((sint32*) &OneContext.thick), '>'); // ◀

				if(Buffer::CountOf(TempBuffer) < EdgeSize)
					TempBuffer = Buffer::Realloc(NEEDS TempBuffer, EdgeSize);
				for(sint32 e = 0; e < EdgeCount; ++e)
				{
					OneShape->GetEdge(e)->Write((uint08*) TempBuffer);
					N1D7::WriteBinaries(Bits, (bytes) TempBuffer, EdgeSize); // ◀
				}

				// Crc
				const sint32 DataCrc = 0xFFFFFFFF; ////////////////////
				WriteChunkString(Bits, "<CRC", '/'); // ◀
				WriteChunkNumber(Bits, DataCrc, '>'); // ◀
			}
		}
		Buffer::Free(TempBuffer);

		// Continue, End
		if(HasBreak) WriteChunkString(Bits, "<CONTINUE", '>'); // ◀
		else
		{
			WriteChunkString(Bits, "<END", '>'); // ◀
			document->LastMS = -1;
		}

		// ■ Patterning
		while(Bits < BitsEnd)
		{
			const sint32 CopyLength = Math::Min(Bits - BitsBegin, BitsEnd - Bits);
			Memory::Copy(Bits, BitsBegin, CopyLength);
			Bits += CopyLength;
		}

		// ■ Save bitmap
		void* FileW = File::OpenForWrite(filename);
		File::Write(FileW, (bytes) "BM", 2);
		File::Write(FileW, Bmp, BmpSize);
		File::Close(FileW);

		Buffer::Free((buffer) Bmp);
		return (HasBreak)? ShapeFocus : 0;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    PX_API(api_flag, Export_MergeSkin, api_string filename, api_string bmpFilename)
    PX_API_IMPL(return Export::MergeSkin(PX_API_STR(filename), PX_API_STR(bmpFilename)))
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool Export::MergeSkin(chars filename, chars bmpFilename)
	{
		// Load doc
		const void* DocFileR = File::OpenForRead(filename);
		if(DocFileR == nullptr)
		{
			PX_ASSERT("파일이 존재하지 않습니다", false);
			return false;
		}
		const sint32 DocSize = File::GetSize(DocFileR) - 2;
		uint08* Doc = (uint08*) Buffer::Alloc(NEEDS DocSize, true);
		uint08* DocBits = (uint08*) (Doc + sizeof(BitmapFile) + sizeof(BitmapInfo));
		const sint32 DocRow = (BytesPerPixel * BitsWidth + 3) & ~3;
		File::Skip(DocFileR, 2);
		File::Read(DocFileR, Doc, DocSize);
		File::Close(DocFileR);

		// Load skin
		const void* SkinFileR = File::OpenForRead(bmpFilename);
		if(SkinFileR == nullptr)
		{
			PX_ASSERT("스킨파일이 존재하지 않습니다", false);
			Buffer::Free((buffer) Doc);
			return false;
		}
		sint08 BmCode[2];
		File::Read(SkinFileR, (uint08*) BmCode, 2);
		if(BmCode[0] != 'B' || BmCode[1] != 'M')
		{
			PX_ASSERT("스킨파일은 비트맵만 가능합니다", false);
			File::Close(SkinFileR);
			Buffer::Free((buffer) Doc);
			return false;
		}
		const sint32 SkinSize = File::GetSize(SkinFileR) - 2;
		uint08* Skin = (uint08*) Buffer::Alloc(NEEDS SkinSize, true);
		File::Read(SkinFileR, Skin, SkinSize);
		File::Close(SkinFileR);

		// Merging
		const BitmapInfo* SkinInfoHeader = (BitmapInfo*) (Skin + sizeof(BitmapFile));
		PX_ASSERT("스킨파일은 24/32비트 비트맵만 가능합니다",
			SkinInfoHeader->bitcount == 24 || SkinInfoHeader->bitcount == 32);
		bytes SkinBits = (uint08*) (Skin + sizeof(BitmapFile) + sizeof(BitmapInfo));
		const sint32 SkinBytesPerPixel = SkinInfoHeader->bitcount / 8;
		const sint32 SkinWidth = SkinInfoHeader->width;
		const sint32 SkinHeight = SkinInfoHeader->height;
		const sint32 SkinRow = (SkinBytesPerPixel * SkinInfoHeader->width + 3) & ~3;
		for(sint32 y = 0; y < BitsHeight; ++y)
		{
			bytes CurSkinBitsForY = &SkinBits[y * SkinHeight / BitsHeight * SkinRow];
			uint08* CurDocBits = &DocBits[y * DocRow];
			for(sint32 x = 0; x < BitsWidth; ++x)
			{
				bytes CurSkinBits = &CurSkinBitsForY[x * SkinWidth / BitsWidth * SkinBytesPerPixel];
				CurDocBits[0] &= (CurSkinBits[0] & (CurSkinBits[0] << 1)) | 0x7F;
				CurDocBits[1] &= (CurSkinBits[1] & (CurSkinBits[1] << 1)) | 0x7F;
				CurDocBits[2] &= (CurSkinBits[2] & (CurSkinBits[2] << 1)) | 0x7F;
				CurDocBits += BytesPerPixel;
			}
		}
		Buffer::Free((buffer) Skin);

		// Save doc
		void* FileW = File::OpenForWrite(filename);
		File::Write(FileW, (bytes) "BM", 2);
		File::Write(FileW, Doc, DocSize);
		File::Close(FileW);
		Buffer::Free((buffer) Doc);
		return true;
	}
}
