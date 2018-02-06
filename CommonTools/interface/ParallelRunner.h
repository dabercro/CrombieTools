/**
   @file ParallelRunner.h

   Defines the ParallelRunner class

   @author Daniel Abercrombie <dabercro@mit.edu>
*/

#ifndef CROMBIETOOLS_COMMONTOOLS_PARALLELRUNNER_H
#define CROMBIETOOLS_COMMONTOOLS_PARALLELRUNNER_H

#include <vector>
#include <queue>

#include "TMutex.h"
#include "TThread.h"

#include "FileConfigReader.h"

TMutex queue_lock;
TMutex output_lock;

/**
   @ingroup commongroup
   @class ParallelRunner
   Class for running over FileInfos in parallel
*/

class ParallelRunner : public FileConfigReader {
 public:
  /// Set the number of cores to use in the plotting
  void SetNumThreads ( UInt_t nthreads ) { fNumThreads = nthreads; }

 protected:
  /// Run over all of the filled FileInfos
  void RunThreads ();

  /// Virtual function that must be implemented in inheriting class
  virtual void RunFile (FileInfo& info) = 0;

 private:
  /// Number of cores to prepare plots with
  UInt_t fNumThreads {1};

  /// Files to run over in parallel
  std::priority_queue<FileInfo>  file_queue;

  /// Runs a single thread over files
  static void* RunThread (void* prep);
};

void ParallelRunner::RunThreads() {
  for (auto type : gFileTypes) {
    const auto& infos = *(GetFileInfo(type));
    for (auto info : infos)
      file_queue.push(*info);
  }

  if (not fNumThreads)
    fNumThreads++;

  std::vector<TThread*> threads;
  for (decltype(fNumThreads) i_thread = 0; i_thread < fNumThreads; ++i_thread) {
    TThread* temp = new TThread(RunThread, this);
    threads.push_back(temp);
    temp->Run(this);
  }

  for (auto thread : threads) {
    thread->Join();
    delete thread;
  }
}

void* ParallelRunner::RunThread(void* prep) {
  auto* runner = reinterpret_cast<ParallelRunner*>(prep);
  bool running = true;
  FileInfo info;
  while(true) {
    queue_lock.Lock();
    running = !runner->file_queue.empty();
    if (running) {
      info = runner->file_queue.top();
      runner->file_queue.pop();
    }
    queue_lock.UnLock();

    if (not running)
      break;

    runner->RunFile(info);
  }
  return nullptr;
}

#endif
