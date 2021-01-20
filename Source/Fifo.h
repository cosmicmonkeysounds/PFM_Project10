/*
  ==============================================================================

    Fifo.h
    Created: 16 Jan 2021 12:23:02am
    Author:  l33t h4x0r

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

template<typename T>
struct Fifo
{
    
    void prepare( int numberOfChannels, int numberOfSamples )
    {
    
        for( auto& buffer : buffers )
        {
            buffer.setSize( numberOfChannels, numberOfSamples );
        }
        
    }
    
    bool push( const T& itemToAdd )
    {
        auto scopedWriter = fifo.write(1);
        
        if( scopedWriter.blockSize1 >= 1 )
        {
            return true;
        }
        
        return false;
    }
    
    bool pull( T& itemToPull )
    {
        auto scopedReader = fifo.read(1);
        
        if( scopedReader.blockSize1 >= 1 )
        {
            return true;
        }
        
        return false;
    }
    
private:
    
    static constexpr int Size = 5;
    std::array<T, Size> buffers;
    AbstractFifo fifo{Size};
    
};
