/* File: tmkpy.cpp */

#include <tmkpy.h>

#include <tmk/cpp/hashing/filehasher.h>
#include <iostream>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <memory>
#include <math.h>

using namespace std;
using namespace facebook::tmk;
using namespace facebook::tmk::algo;
using namespace facebook::tmk::io;


TMKFeatureVectors hashVideo(std::string inputVideoFileName, std::string ffmpegPath) {
	int resampleFramesPerSecond = 15; // TMK default
	std::string frameFeatureAlgorithmName = "pdqf";

	bool verbose = false;
	
	TMKFeatureVectors tmkFeatureVectors;
	
	TMKFramewiseAlgorithm tmkFramewiseAlgorithm = algoFromLowercaseName(frameFeatureAlgorithmName);
	if (tmkFramewiseAlgorithm == TMKFramewiseAlgorithm::UNRECOGNIZED) {
		fprintf(stderr, "%s: unrecognized algorithm name.\n", "pytmk");
		return {};
	}
	
	
	bool rc = facebook::tmk::hashing::hashVideoFile(
		inputVideoFileName,
		tmkFramewiseAlgorithm,
		ffmpegPath,
		resampleFramesPerSecond,
		tmkFeatureVectors,
		verbose,
		"pytmk");
	
	return tmkFeatureVectors;
}

TMKFeatureVectors createTMKFeatureVectors(
		int framesPerSecond, // provenance
		int frameFeatureCount, // informational
		const Periods& periods,
		const FourierCoefficients& fourierCoefficients,
		const FrameFeature& pureAverageFeature,
		const FeaturesByPeriodsAndFourierCoefficients& cosFeatures,
		const FeaturesByPeriodsAndFourierCoefficients& sinFeatures) {
	
	string frameFeatureAlgorithmName = "pdqf";
	
	TMKFeatureVectors tmkFeatureVectors; //std::shared_ptr
	
	TMKFramewiseAlgorithm tmkFramewiseAlgorithm = algoFromLowercaseName(frameFeatureAlgorithmName);

	tmkFeatureVectors=*(TMKFeatureVectors::tryCreateFromPrecomputed(
		tmkFramewiseAlgorithm, // provenance
		framesPerSecond, // provenance
		frameFeatureCount, // informational
		periods,
		fourierCoefficients,
		pureAverageFeature,
		cosFeatures,
		sinFeatures));

	return tmkFeatureVectors;
}

//Methods to query a haystack and compute level-2 scores

//Default to 1 thread if not given
vector<float> query(TMKFeatureVectors needle, vector<string> haystackFilenames) {
	return query(needle,haystackFilenames,1);
}
//Accept a string as input for needle
vector<float> query(string needle, vector<string> haystackFilenames) {
	return query(needle,haystackFilenames,1);
}
vector<float> query(string needle, vector<string> haystackFilenames, int numThreads) {
	shared_ptr<TMKFeatureVectors> vid1=TMKFeatureVectors::readFromInputFile(needle.c_str(),"tmkpy");
	if (vid1==nullptr) {
		fprintf(stderr, "tmkpy: failed to read needle \"%s\".\n", needle.c_str());
		throw std::invalid_argument("Failed to read needle from supplied filename");
		//return {}; //Returns empty tuple in Python
	} else {
		return query(*vid1,haystackFilenames,numThreads);
	}
}

//Version with possibly more than one thread
vector<float> query(TMKFeatureVectors needle, vector<string> haystackFilenames, int numThreads) {

	//initalize return vector with -1's that is the same lenth as haystackFilenames
	vector<float> level2scores(haystackFilenames.size(), -1);
	
	if (numThreads<=1) {
		_query_worker(needle,haystackFilenames,level2scores,0,haystackFilenames.size());
	} else {
		if (numThreads>haystackFilenames.size()) {
			numThreads=haystackFilenames.size(); //No larger than the haystack length
		}
		int haystackSize=haystackFilenames.size();
		int numPerThread=(int)ceil(haystackSize/(double)numThreads);
		int start,end;
		std::vector<std::thread> threads;
		for (int i=0; i<numThreads; i++) {
			start=i*numPerThread;
			end=(i+1)*numPerThread;
			if (start>haystackSize) continue;
			if (end>haystackSize) end=haystackSize;
			threads.push_back(std::thread(_query_worker,needle,std::ref(haystackFilenames),std::ref(level2scores),start,end));
		}
		//Wait for all threads to finish
		for (auto &th : threads) {
			th.join();
		}
	}

	return level2scores;

}

void _query_worker(TMKFeatureVectors needle, vector<string> haystackFilenames, vector<float> &level2scores, int start, int end) {
	//fprintf(stderr,"_query_worker: %i %i\n",start,end); //TODO: Temporary debug info
	shared_ptr<TMKFeatureVectors> vid2;
	for (int i=start; i<end; i++) {
		//fprintf(stderr,"_query_worker: %i %s\n",i,haystackFilenames[i].c_str());
		vid2=TMKFeatureVectors::readFromInputFile(haystackFilenames[i].c_str(),"tmkpy");
		if (vid2==nullptr) {
			fprintf(stderr, "tmkpy: failed to read \"%s\".\n", haystackFilenames[i].c_str());
			continue;
		}
		//Now have a valid TMKFeatureVector. Compute L2 score
		level2scores[i]=TMKFeatureVectors::computeLevel2Score(needle,*vid2);
		
	}
}
