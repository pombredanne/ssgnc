#ifndef SSGNC_NGRAM_INDEX_H
#define SSGNC_NGRAM_INDEX_H

#include "file-map.h"

namespace ssgnc {

class NgramIndex
{
public:
	class FileEntry
	{
	public:
		FileEntry() : file_id_(0), offset_lo_(0), offset_hi_(0) {}

		bool set_file_id(Int32 file_id) SSGNC_WARN_UNUSED_RESULT;
		bool set_offset(UInt32 offset) SSGNC_WARN_UNUSED_RESULT;

		Int32 file_id() const { return file_id_; }
		UInt32 offset() const
		{ return (static_cast<UInt32>(offset_hi_) << 16) + offset_lo_; }

	private:
		Int16 file_id_;
		UInt16 offset_lo_;
		UInt16 offset_hi_;
	};

	class Entry
	{
	public:
		Entry() : file_id_(0), offset_(0), approx_size_(0) {}

		bool set_file_id(Int32 file_id) SSGNC_WARN_UNUSED_RESULT;
		bool set_offset(UInt32 offset) SSGNC_WARN_UNUSED_RESULT;
		bool set_approx_size(Int64 approx_size) SSGNC_WARN_UNUSED_RESULT;

		Int32 file_id() const { return file_id_; }
		UInt32 offset() const { return offset_; }
		Int64 approx_size() const { return approx_size_; }

	private:
		Int32 file_id_;
		UInt32 offset_;
		Int64 approx_size_;
	};

	enum { MAX_FILE_ID = 9999 };
	enum { MAX_OFFSET = 0x7FFFFFFF };

	static const Int64 MAX_APPROX_SIZE = 1LL << 40;

public:
	NgramIndex();
	~NgramIndex() { clear(); }

	void clear();

	Int32 max_num_tokens() const { return max_num_tokens_; }
	Int32 max_token_id() const { return max_token_id_; }

	bool get(Int32 num_tokens, Int32 token_id, Entry *entry)
		SSGNC_WARN_UNUSED_RESULT;

	bool load(const Int8 *path) SSGNC_WARN_UNUSED_RESULT;
	bool read(std::istream *in) SSGNC_WARN_UNUSED_RESULT;

	bool mmap(const Int8 *path) SSGNC_WARN_UNUSED_RESULT;
	bool map(const void *ptr, UInt32 size) SSGNC_WARN_UNUSED_RESULT;

private:
	Int32 max_num_tokens_;
	Int32 max_token_id_;
	const FileEntry *entries_;
	std::vector<FileEntry> entries_buf_;
	FileMap file_map_;

	bool readData(std::istream *in) SSGNC_WARN_UNUSED_RESULT;
	bool mapData(const void *ptr, UInt32 size) SSGNC_WARN_UNUSED_RESULT;

	// Disallows copies.
	NgramIndex(const NgramIndex &);
	NgramIndex &operator=(const NgramIndex &);
};

inline bool NgramIndex::FileEntry::set_file_id(Int32 file_id)
{
	if (file_id < 0 || file_id > MAX_FILE_ID)
	{
		SSGNC_ERROR << "Out of range file ID: " << file_id << std::endl;
		return false;
	}
	file_id_ = static_cast<Int16>(file_id);
	return true;
}

inline bool NgramIndex::FileEntry::set_offset(UInt32 offset)
{
	if (offset > MAX_OFFSET)
	{
		SSGNC_ERROR << "Too large offset: " << offset << std::endl;
		return false;
	}
	offset_lo_ = static_cast<UInt16>(offset & 0xFFFF);
	offset_hi_ = static_cast<UInt16>(offset >> 16);
	return true;
}

inline bool NgramIndex::Entry::set_file_id(Int32 file_id)
{
	if (file_id < 0 || file_id > MAX_FILE_ID)
	{
		SSGNC_ERROR << "Out of range file ID: " << file_id << std::endl;
		return false;
	}
	file_id_ = file_id;
	return true;
}

inline bool NgramIndex::Entry::set_offset(UInt32 offset)
{
	if (offset > MAX_OFFSET)
	{
		SSGNC_ERROR << "Too large offset: " << offset << std::endl;
		return false;
	}
	offset_ = offset;
	return true;
}

inline bool NgramIndex::Entry::set_approx_size(Int64 approx_size)
{
	if (approx_size < 0 || approx_size > MAX_APPROX_SIZE)
	{
		SSGNC_ERROR << "Out of range approximate size: "
			<< approx_size << std::endl;
		return false;
	}
	approx_size_ = approx_size;
	return true;
}

}  // namespace ssgnc

inline ssgnc::Int64 operator-(const ssgnc::NgramIndex::FileEntry &lhs,
	const ssgnc::NgramIndex::FileEntry &rhs)
{
	return ((static_cast<ssgnc::Int64>(lhs.file_id()) << 31) + lhs.offset())
		- ((static_cast<ssgnc::Int64>(rhs.file_id()) << 31) + rhs.offset());
}

#endif  // SSGNC_NGRAM_INDEX_H
