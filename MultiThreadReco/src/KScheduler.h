/*
 * KScheduler.h
 *
 * also includes implementation of KJob
 * 
 * Author: Noah Wuerfel, nwuerfel@umich.edu
 * Created: 10-19-2020
 * AP AP AP AP
 *
 *
 */

#ifndef _KSCHEDULER_H_
#define _KSCHEDULER_H_

#include <GlobalConsts.h>
#include <geom_svc/GeomSvc.h>

#include <list>
#include <map>
#include <queue>
#include <new>

#include <TString.h>
#include <TRandom.h>
#include <TClonesArray.h>
#include <TStopwatch.h>
#include <TThread.h>
#include <TSemaphore.h>
#include <TMutex.h>
#include <TCondition.h>

#include <ktracker/EventReducer.h>
#include <ktracker/SRawEvent.h>
#include <ktracker/TriggerAnalyzer.h>
#include <ktracker/KalmanFastTracking.h>
#include <ktracker/UtilSRawEvent.h>
#include <ktracker/GFFitter.h>


#define NTHREADS 16 // later make JobOpt
#define INPUT_PIPE_DEPTH 32 // play with iobuffer to load memory vs do computation...
#define OUTPUT_PIPE_DEPTH 32

#define NEVENT_REDUCERS NTHREADS // reducer factories
#define NKFAST_TRACKERS NTHREADS // tracking factories

class PHField;
class TGeoManager;
class KalmanFitter;

class KJob;
class SQEvent;
class SQHitMap;
class SQHitVector;

class KScheduler {
public:
    KScheduler(TString inFile, TString outFile);
    ~KScheduler();

    void Init(PHField* phfield, TGeoManager*t_geo_manager, KalmanFitter* kfitter, SQGenFit::GFFitter* gfitter);

    static TString getInputFilename(); 
    static void    setInputFilename(TString name); 
    static TString getOutputFilename();
    static void    setOutputFilename(TString name);
    //static void getTrackletsInStation(int stid);
    void postCompletedEvent();

    Int_t runThreads();
    Int_t endThreads();
    void PushEvent(SRawEvent* sraw, bool copy);
    void PushPoison();

    static void Verbose(const int a) { verb = a; }
    static int  Verbose() { return verb; }

    static void PrintFreq(const int i) { print_freq = i; }
    static int  PrintFreq()     { return print_freq; }

    static void SaveNum(const int i) { save_num = i; }
    static int  SaveNum()     { return save_num; }

    static void SaveRawEvent(const bool save) { save_raw_evt = save; }
    static bool SaveRawEvent()         { return save_raw_evt; }

    //void UseE906Data(const bool a) { use_e906_data = a; }
    //bool UseE906Data() { return use_e906_data; }

    void UseTrackletReco(const bool a) { use_tracklet_reco = a; }
    bool UseTrackletReco() const { return use_tracklet_reco; }

    KalmanFitter* GetKFitter() { return m_kfitter; }
    SQGenFit::GFFitter* GetGFitter() { return m_gfitter; }

    // output tracklets
    
    // tracklet outputs mutex
    TMutex* ktrkQueueMutex;

    // tracklets
    TSemaphore* ktrkqFSem;
    TSemaphore* ktrkqESem;

    // tracklet arrays...
    std::queue<TClonesArray*> kTrackArrQueue;

private:

    // reader
    //static void* fReaderThread(void* readerArg);
    //Int_t startReaderThread();
    //Int_t stopReaderThread();
    
    // reaper
    static void* fReaperThread(void* reaperArg);
    Int_t startReaperThread();
    Int_t stopReaperThread();

    // worker threads
    static void* fWorkerThread(void* wArg);
    Int_t startWorkerThread(unsigned threadId);
    Int_t startWorkerThreads();

    bool fitTrackCand(Tracklet& tracklet, KalmanFitter* fitter      , SRecTrack& strack);
    bool fitTrackCand(Tracklet& tracklet, SQGenFit::GFFitter* fitter, SRecTrack& strack);

// stuff
    static int verb;
    static int print_freq;
    static int save_num;
    static bool save_raw_evt;
    //static bool use_e906_data;

    bool use_tracklet_reco;

    TriggerAnalyzer* p_triggerAna;

    //io thread
    static TString inputFilename;
    static TString outputFilename;

    // NOT PROTECTED ASSUMES ONE fREAPER
    static int completedEvents;
    static Double_t totalTimeElapsed;

    TStopwatch*  avgTimer;
    TStopwatch* totalTimer;

    //static TString outputFile;
    // reader
    TThread* fRDPtr;
    // reaper
    TThread* fRPPtr;

    // shooting in the dark with root multithreading...
    TMutex* wArgMutex;
    TMutex* fReaderMutex; 

    // reader 
    TMutex* newJobQueuePutMutex;
    TMutex* newJobQueueTakeMutex;

    // reducer
    TMutex* evRedQueuePutMutex;
    TMutex* evRedQueueTakeMutex;

    // tracker
    TMutex* kFTrkQueuePutMutex;
    TMutex* kFTrkQueueTakeMutex;

    // reaper
    TMutex* cmpJobQueuePutMutex;
    TMutex* cmpJobQueueTakeMutex;

    // input pipe
    std::queue<KJob*> newJobQueue; 

    // output pipe
    std::queue<KJob*> cmpJobQueue;


    // first stage sems
    TSemaphore* njqFSem;
    TSemaphore* njqESem;

    // secondstage pipielines
    TSemaphore* erqFSem;
    TSemaphore* erqESem;

    // finder sems
    TSemaphore* kftqFSem;
    TSemaphore* kftqESem;
    // out stage sems
    TSemaphore* cjqFSem;
    TSemaphore* cjqESem;

    // worker threads
    // TODO split into levels: eventReducer...
    std::array<TThread*, NTHREADS> workThreadArr;

    // event reducers.,.
    std::queue<EventReducer*> eventReducerQueue;
    // fast trackers...
    std::queue<KalmanFastTracking*> kFastTrkQueue;

    KalmanFitter* m_kfitter;
    SQGenFit::GFFitter* m_gfitter;
};

#endif 
