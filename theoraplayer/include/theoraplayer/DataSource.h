/// @file
/// @version 2.0
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause
/// 
/// @section DESCRIPTION
/// 
/// Provides an interface for a video data source.

#ifndef THEORAPLAYER_DATA_SOURCE_H
#define THEORAPLAYER_DATA_SOURCE_H

#include <stdint.h>
#include <limits.h>
#include <string>

#include "theoraplayerExport.h"

// TODOth - split these into separate files

namespace theoraplayer
{
	/**
		This is a simple class that provides abstracted data feeding. You can use the
		TheoraFileDataSource for regular file playback or you can implement your own
		internet streaming solution, or a class that uses encrypted datafiles etc.
		The sky is the limit
	*/
	class theoraplayerExport DataSource
	{
	public:
		DataSource();
		virtual ~DataSource();
		/**
			Reads nBytes bytes from data source and returns number of read bytes.
			if function returns less bytes then nBytes, the system assumes EOF is reached.
		*/
		virtual int read(void* output, int nBytes) = 0;
		//! returns a string representation of the DataSource, eg 'File: source.ogg'
		virtual std::string toString() = 0;
		//! position the source pointer to byte_index from the start of the source
		virtual void seek(uint64_t byte_index) = 0;
		//! return the size of the stream in bytes
		virtual uint64_t getSize() = 0;
		//! return the current position of the source pointer
		virtual uint64_t getPosition() = 0;

	};

	/**
		provides standard file IO
	*/
	class theoraplayerExport FileDataSource : public DataSource
	{
	public:
		FileDataSource(std::string filename);
		~FileDataSource();

		uint64_t getSize();
		uint64_t getPosition();
		std::string getFilename() { return this->filename; }

		std::string toString() { return this->filename; }

		int read(void* output, int nBytes);
		void seek(uint64_t byte_index);

	private:
		FILE* filePtr;
		std::string filename;
		uint64_t length;

		void openFile();

	};

	/**
		Pre-loads the entire file and streams from memory.
		Very useful if you're continuously displaying a video and want to avoid disk reads.
		Not very practical for large files.
	*/
	class theoraplayerExport MemoryDataSource : public DataSource
	{
	public:
		MemoryDataSource(unsigned char* data, long size, const std::string& filename = "memory");
		MemoryDataSource(std::string filename);
		~MemoryDataSource();

		uint64_t getSize();
		uint64_t getPosition();
		std::string getFilename() { return this->filename; }

		int read(void* output, int bytes);
		void seek(uint64_t byte_index);

		inline std::string toString() { return "MEM:" + this->filename; }

	private:
		std::string filename;
		uint64_t length;
		uint64_t readPointer;
		unsigned char* data;

	};

}
#endif
