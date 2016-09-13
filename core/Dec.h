// -*- C++ -*-
// 

#pragma once

#include "base.h"
#include "Parser.h"
#include "MsgObserver.h"

namespace ACT
{
   class DecoderBase
   {
   public:
      DecoderBase () :
         nMsgsIgnored (0),
         nStrayBytes (0) { }

      virtual ~DecoderBase () { }

   protected:
      void onUnknownMessage (u32 type);
      void onStrayBytes (u32 len);

      void dumpStats () const; // at 'eof'

      u32 nMsgsIgnored;
      u32 nStrayBytes;
   };

   // ////////////////////////////////////////////////////////////////
   // Binary

   class BinDecoder : public DecoderBase,
                      public BinParser::Observer
   {
   public:
      BinDecoder (MsgObserver & obs) :
         DecoderBase (),
         observer (obs),
         parser (new BinParser (* this)) { }

      ~BinDecoder ()
      {
         delete parser;
      }

      void run ();

   private:
      // From Parser:
      void onMessage (u32 msgLen, const u8*) override;
      
      // field decoders:
      u16 decodeU16 (const u8*, u32 off) const;
      u32 decodeU32 (const u8*, u32 off) const;
      u64 decodeU64 (const u8*, u32 off) const;
      i64 decodeI64 (const u8*, u32 off) const;
      std::string decodeString (const u8*, u32 off, u32 len) const;

      // msg decoders:
      u32 decodeSystemStatus (const u8*);
      u32 decodeInstrumentInfo (const u8*);
      u32 decodeInstrumentStatus (const u8*);
      u32 decodeIndexInfo (const u8*);
      u32 decodeIndexMemberInfo (const u8*);
      u32 decodeIndexUpdate (const u8*);
      u32 decodeOrderInserted (const u8*);
      u32 decodeOrderDeleted (const u8*);
      u32 decodeOrderReduced (const u8*);
      u32 decodeOrderReplaced (const u8*);
      u32 decodeOrderExecuted (const u8*);
      u32 decodeTopOfBook (const u8*);
      u32 decodeTradeStats (const u8*);

      u32 decodeInsertOrderReply (const u8*);
      u32 decodeDeleteOrderReply (const u8*);
      u32 decodeReduceOrderReply (const u8*);
      u32 decodeReplaceOrderReply (const u8*);
      u32 decodeRejectOrderReply (const u8*);

      u32 decodeInsertOrder (const u8*);
      u32 decodeDeleteOrder (const u8*);
      u32 decodeReduceOrder (const u8*);
      u32 decodeReplaceOrder (const u8*);

      //
      void copyHeader (const MsgBase & from, MsgBase & to)
      {
         to.msgSize = from.msgSize;
         to.msgType = from.msgType;
         to.msgSeqNo = from.msgSeqNo;
         to.msgTsp = from.msgTsp;
      }

      void assertMsgSize (const MsgBase & m, u32 size) const;

      MsgObserver& observer;
      BinParser* parser;
      MsgBase msgBase;
   };

   // //////////////////////////////////////////////////////////////////

   class TextDecoder : public DecoderBase,
                       public TextParser::Observer
   {
   public:
      TextDecoder (MsgObserver & obs) :
         DecoderBase (),
         observer (obs),
         parser (new TextParser (* this)),
         msgBuf (nullptr),
         msgOff (0),
         msgLen (0) { }

      ~TextDecoder ()
      {
         delete parser;
      }

      void run ();

   private:
      // From Parser:
      void onMessage (const u8* data, u32 len);

      // field decoders:
      template<typename T> void decode (T &);
      template<typename T> void decodeSigned (T &);
      void decodeStr (std::string &);
      void decodeFloat (u64 & v);

      // msg decoders:
      void decodeSystemStatus ();
      void decodeInstrumentInfo ();
      void decodeInstrumentStatus ();
      void decodeIndexInfo ();
      void decodeIndexMemberInfo ();
      void decodeIndexUpdate ();
      void decodeOrderInserted ();
      void decodeOrderDeleted ();
      void decodeOrderReduced ();
      void decodeOrderReplaced ();
      void decodeOrderExecuted ();
      void decodeTopOfBook ();
      void decodeTradeStats ();

      void decodeInsertOrderReply ();
      void decodeDeleteOrderReply ();
      void decodeReduceOrderReply ();
      void decodeReplaceOrderReply ();
      void decodeRejectOrderReply ();

      void decodeInsertOrder ();
      void decodeDeleteOrder ();
      void decodeReduceOrder ();
      void decodeReplaceOrder ();

      // 
      void copyHeader (const MsgBase & from, MsgBase & to)
      {
         to.msgSize = from.msgSize;
         to.msgType = from.msgType;
         to.msgSeqNo = from.msgSeqNo;
         to.msgTsp = from.msgTsp;
      }

      void skipSpaces ();

      MsgObserver & observer;
      TextParser* parser;
      MsgBase msgBase;
      const u8* msgBuf;
      u32 msgOff;
      u32 msgLen;
   };

}
