//tmkpy.h

#include <tmk/cpp/algo/tmkfv.h>
#include <tmk/cpp/io/tmkio.h>

using namespace std;
using namespace facebook::tmk;
using namespace facebook::tmk::algo;

//Hash a video and return the TMKFeatureVectors object
TMKFeatureVectors hashVideo(string inputVideoFileName, string ffmpegPath);

//Re-create a TMKFeatureVectors object from all the underlying data. Assume the algorithm used is "pdqf"
TMKFeatureVectors createTMKFeatureVectors(
      int framesPerSecond, // provenance
      int frameFeatureCount, // informational
      const Periods& periods,
      const FourierCoefficients& fourierCoefficients,
      const FrameFeature& pureAverageFeature,
      const FeaturesByPeriodsAndFourierCoefficients& cosFeatures,
      const FeaturesByPeriodsAndFourierCoefficients& sinFeatures);

//Methods to query a hackstack and compute level-2 scores. Versions without numThreads simlpy call the other version with numThreads=1

//With needle as a string pointing to a .tmk file
vector<float> query(string needle, vector<string> haystackFilenames, int numThreads);
vector<float> query(string needle, vector<string> haystackFilenames);

//Or with needle as a TMKFeatureVector itself
vector<float> query(TMKFeatureVectors needle, vector<string> haystackFilenames);
vector<float> query(TMKFeatureVectors needle, vector<string> haystackFilenames, int numThreads);


//Internal worker function. Private.
void _query_worker(TMKFeatureVectors needle, vector<string> haystackFilenames, vector<float> &level2scores, int start, int end);

