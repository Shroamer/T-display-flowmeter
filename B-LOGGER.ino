// add new pulsesPerMinuteLogAvg entry to logArray if newLogData rised 
void processLogStore() {
  if (newLogData) {
    newLogData = 0;                                           // reset flag to be rised by logTimer isr
    if (logArrayIndex < logArrayLength - 2) logArrayIndex++;  // iterate log index
    else logArrayIndex = 0;                                   // or reset log index if overflow
    logArray[logArrayIndex] = pulsesPerMinuteLogAvg;          // storing last calculated log entry value
  }
}