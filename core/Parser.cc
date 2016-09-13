// -*- C++ -*-
// 

#include "Parser.h"

using namespace ACT;

// /////////////////////////////////////////////////////////////

void
BinParser::parse (const u8* data, u32 len)
{
   if (partialReadCount > 0)
   {
      while (len > 0)
      {
         const u8* d = data;
         ++ data;
         -- len;
         ++ msgBytesRead;
         
         if (state == State::RLB2)
         {
            msgLen |= ((* d) << 8);
            if (msgLen < 16)
            {
               std::cerr << "BinParser: Frame too short: len="
                         << msgLen << std::endl;
               abort ();
            }
            state = State::RMSG;
            partialReadBuffer [partialReadCount ++] = * d;
         }
         else if (state == State::RMSG)
         {
            partialReadBuffer [partialReadCount ++] = * d;
               
            if (msgBytesRead == msgLen)
            {
               observer.onMessage (msgLen, & partialReadBuffer [2]);
               msgLen = 0;
               msgBytesRead = 0;
               state = State::RLB1;
               partialReadCount = 0;
               break;
            }
         }
      }
   }

   size_t lastPos = 0;

   for (size_t p1 = 0; p1 < len; ++ p1)
   {
      ++ msgBytesRead;

      if (state == State::RLB1)
      {
         msgLen = data [p1];
         state = State::RLB2;
      }
      else if (state == State::RLB2)
      {
         msgLen |= ((data [p1]) << 8);

         if (msgLen < 16)
         {
            std::cerr << "BinParser: Frame too short: len="
                      << msgLen << std::endl;
            abort ();
         }
         if (msgLen > 128)
         {
            std::cerr << "BinParser: Frame too long: len="
                      << msgLen << std::endl;
            abort ();
         }
         state = State::RMSG;
      }
      else if (state == State::RMSG)
      {
         if (msgBytesRead == msgLen)
         {
            observer.onMessage (msgLen, & data [lastPos + 2]);

            lastPos += msgBytesRead;

            // reset msg state
            msgLen = 0;
            msgBytesRead = 0;
            state = State::RLB1;
         }
      }
   }

   len -= lastPos;

   if (len > 0)
   {
      memcpy (partialReadBuffer, data + lastPos, len);
      partialReadCount = len;
   }

}

// /////////////////////////////////////////////////////////////

void
TextParser::parse (const u8* data, u32 len)
{
   if (partialReadCount > 0)
   {
      while (len > 0)
      {
         const u8* d = data;

         data ++;
         len --;

         if (* d == '\n')
         {
            observer.onMessage (partialReadBuffer, partialReadCount);
            partialReadCount = 0;
            break;
         }

         partialReadBuffer [partialReadCount ++] = * d;
      }
   }

   size_t lastPos = 0;
   size_t frameLen;

   for (size_t p1 = 0; p1 < len; ++p1)
   {
      if (data[p1] == '\n')
      {
         frameLen = p1 - lastPos;
         observer.onMessage (data + lastPos, frameLen);
         lastPos += frameLen+1; // Skip separator
      }
   }

   len -= lastPos;

   if (len > 0)
   {
      memcpy (partialReadBuffer, data + lastPos, len);
      partialReadCount= len;
   }

}
