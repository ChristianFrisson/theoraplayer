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
/// Defines a video manager.

#ifndef THEORAPLAYER_VIDEO_MANAGER_H
#define THEORAPLAYER_VIDEO_MANAGER_H

#include <vector>
#include <list>
#include <string>

#include "theoraplayerExport.h"
#include "VideoClip.h"



namespace theoraplayer
{
	// forward class declarations
	class AudioInterfaceFactory;
	class DataSource;
	class Mutex;
	class WorkerThread;

	/**
		This is the main singleton class that handles all playback/sync operations
	*/
	class theoraplayerExport Manager
	{
	public:
		friend class WorkerThread;

		Manager(int workerThreadCount);
		virtual ~Manager();

		int getWorkerThreadCount();
		void setWorkerThreadCount(int value);
		inline int getDefaultPrecachedFramesCount() { return this->defaultPrecachedFramesCount; }
		inline void setDefaultPrecachedFramesCount(int value) { this->defaultPrecachedFramesCount = value; }
		//! returns the supported decoders (eg. Theora, AVFoundation...)
		std::vector<std::string> getSupportedDecoders();
		//! get nicely formated version string
		std::string getVersionString();
		//! brief get version numbers
		void getVersion(int* major, int* minor, int* revision);

		inline AudioInterfaceFactory* getAudioInterfaceFactory() { return this->audioInterfaceFactory; }
		/**
		\brief you can set your own log function to recieve theora's log calls

		This way you can integrate libtheoraplayer's log messages in your own
		logging system, prefix them, mute them or whatever you want
		*/
		inline void setAudioInterfaceFactory(AudioInterfaceFactory* value) { this->audioInterfaceFactory = value; }

		//! search registered clips by name
		VideoClip* getVideoClipByName(const std::string& name);

		VideoClip* createVideoClip(const std::string& filename, TheoraOutputMode outputMode = TH_RGB, int precachedFramesCountOverride = 0, bool usePotStride = false);
		VideoClip* createVideoClip(DataSource* dataSource, TheoraOutputMode outputMode = TH_RGB, int precachedFramesCountOverride = 0, bool usePotStride = false);
		void destroyVideoClip(VideoClip* clip);

		void update(float timeDelta);

	protected:
		typedef std::vector<VideoClip*> ClipList;
		typedef std::vector<WorkerThread*> ThreadList;

		//! stores pointers to worker threads which are decoding video and audio
		ThreadList workerThreads;
		//! stores pointers to created video clips
		ClipList clips;
		//! stores pointer to clips that were docoded in the past in order to achieve fair scheduling
		std::list<VideoClip*> workLog;
		int defaultPrecachedFramesCount;
		Mutex* workMutex;
		AudioInterfaceFactory* audioInterfaceFactory;

		void _createWorkerThreads(int count);
		void _destroyWorkerThreads();
		/**
		* Called by WorkerThread to request a VideoClip instance to work on decoding
		*/
		VideoClip* _requestWork(WorkerThread* caller);

	};

	extern Manager* manager;

}
#endif