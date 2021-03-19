#pragma once

#include <array>
#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include <glm/vec3.hpp>

#include "core/shared/Const.hpp"

#include "utility/mathlib.hpp"
#include "utility/Color.hpp"

#include "graphics/OpenGL.hpp"

#include "engine/shared/studiomodel/StudioModelFileFormat.hpp"

namespace graphics
{
class TextureLoader;
}

namespace studiomdl
{
//TODO: refactor to use data structures defined by new editable model format
constexpr std::array<std::array<double, 2>, SequenceBlendCount> CounterStrikeBlendRanges{{{-180, 180}, {-45, 45}}};

struct StudioDataDeleter
{
	void operator()(studiohdr_t* pointer) const
	{
		delete[] pointer;
	}

	void operator()(studioseqhdr_t* pointer) const
	{
		delete[] pointer;
	}
};

template<typename T>
using studio_ptr = std::unique_ptr<T, StudioDataDeleter>;

/**
*	Container representing a studiomodel and its data.
*/
class StudioModel final
{
public:
	StudioModel(std::string&& fileName, studio_ptr<studiohdr_t>&& studioHeader, studio_ptr<studiohdr_t>&& textureHeader,
		std::vector<studio_ptr<studioseqhdr_t>>&& sequenceHeaders, bool isDol)
		: _fileName(std::move(fileName))
		, _studioHeader(std::move(studioHeader))
		, _textureHeader(std::move(textureHeader))
		, _sequenceHeaders(std::move(sequenceHeaders))
		, _isDol(isDol)
	{
		assert(_studioHeader);
	}

	~StudioModel() = default;

	StudioModel(const StudioModel&) = delete;
	StudioModel& operator=(const StudioModel&) = delete;

	const std::string& GetFileName() const { return _fileName; }

	void SetFileName(std::string&& fileName)
	{
		_fileName = std::move(fileName);
	}

	studiohdr_t* GetStudioHeader() const { return _studioHeader.get(); }

	bool HasSeparateTextureHeader() const { return !!_textureHeader; }

	studiohdr_t* GetTextureHeader() const
	{
		if (_textureHeader)
		{
			return _textureHeader.get();
		}

		return _studioHeader.get();
	}

	studioseqhdr_t* GetSeqGroupHeader(const size_t i) const { return _sequenceHeaders[i].get(); }

	mstudioanim_t* GetAnim(const mstudioseqdesc_t* pseqdesc) const
	{
		mstudioseqgroup_t* pseqgroup = _studioHeader->GetSequenceGroup(pseqdesc->seqgroup);

		if (pseqdesc->seqgroup == 0)
		{
			return (mstudioanim_t*)((byte*)_studioHeader.get() + pseqgroup->unused2 + pseqdesc->animindex);
		}

		return (mstudioanim_t*)((byte*)_sequenceHeaders[pseqdesc->seqgroup - 1].get() + pseqdesc->animindex);
	}

	bool IsDol() const { return _isDol; }

private:
	std::string _fileName;

	studio_ptr<studiohdr_t> _studioHeader;
	studio_ptr<studiohdr_t> _textureHeader;

	std::vector<studio_ptr<studioseqhdr_t>> _sequenceHeaders;

	bool _isDol;
};
}
