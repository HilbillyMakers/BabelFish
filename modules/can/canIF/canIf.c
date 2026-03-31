#include "canIf.h"

/* 
CAN Interface is supposed to provide the functionality for the can rx/tx handler

Some basic functionality:

#Send single frame
  Parameters: frame (frameID, dlc, data)
  Sends a single frame once

#Send frame periodically
  Parameters: frame (frameID, dlc, data), period(ms)
  Sends a frame periodically
  ?The frame is initially constant. Further changes to the frame can be pondered

#Read single frame
  Parameters: frameID
  Reads the next frame with the matching ID

#Read frame periodically
  Parameters: frameID, period(ms)
  Periodically start listening the frame with the matching ID 

#Read single frame signal
  Parameters: frameID, signal_starting_bit, signal_bit_length
  Reads the next frame with the matching ID and extract the given signal from it

#Read frame signal periodically
  Parameters: frameID, period(ms), signal_starting_bit, signal_bit_length
  Periodically start listening the frame with the matching ID and extract the given signal from it

To improve testing usecases, a bundling functionality can be added:

#Create rx frame bundle
  Parameters: bundle_index
  Creates a bundle of periodically read frames
#Add to rx frame bundle
  Parameters: bundle_index, frameID, period(ms)
  Add frame to a frame bundle. Each frame can have a different read period(ms)
#Get rx bundle frame list
  Parameters: bundle_index
  Get the frame IDs and read periods 

#Create signal bundle - ?Signal bundles behave the same as the frame bundles, but provided a masking layer for more efficient use of the USB data transfer rate
  Parameters: bundle_index
  Creates a bundle of periodically read signals
#Add to rx signal bundle
  Parameters: bundle_index, frameID, signal_start_bit, signal_bit_length, period(ms)
  Add signal to a signal bundle. Each signal can have a different read period(ms)
#Get rx bundle signal list
  Parameters: bundle_index
  Get the frame IDs, signal position, bit length and read periods 
*/