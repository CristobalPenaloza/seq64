/*
 * ============================================================================
 *
 * SeqFile.hpp
 * Class to hold/import/export a Nintendo 64 Music Macro Language (Audioseq) 
 * format sequence file
 * 
 * From seq64 - Sequenced music editor for first-party N64 games
 * Copyright (C) 2014-2021 Sauraen
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * ============================================================================
*/

#pragma once

#include "Common.hpp"

class SeqFile{
public:
    SeqFile(ValueTree abi_);
    ~SeqFile();
    
    static StringArray getAvailABIs();
    static ValueTree loadABI(String name);
    static bool saveABI(String name, ValueTree abi_);
    
    static bool isValidCC(int cc);
    
    //Results are 0 okay, 1 warnings, 2+ errors.
    int importMIDI(File midifile, ValueTree midiopts);
    int exportMIDI(File midifile, ValueTree midiopts);
    #if 0
    int importMus(File musfile, int dialect);
    #endif
    int exportMus(File musfile, int dialect);
    int importCom(File comfile);
    int exportCom(File comfile);
    
    String getInternalString();
    String getDebugOutput();
    
    String seqname;
    
private:
    //For all
    ValueTree abi;
    ValueTree structure;
    StringArray tsecnames;
    int importresult;
    
    String debug_messages;
    CriticalSection debug_mutex;
    void dbgmsg(String s, bool newline = true);
    
    static const int max_layers;
    
    //For importMIDI
    void prefSetBool(ValueTree midiopts, Identifier opt, String value, String prefline);
    void prefSetInt(ValueTree midiopts, Identifier opt, int max, String value, String prefline);
    void prefSetHex(ValueTree midiopts, Identifier opt, int max, String value, String prefline);
    
    MidiMessageSequence* ensureSimulMsgsInOrder(MidiMessageSequence &in);
    
    void getCommandRange(ValueTree command, String meaning, int &range_min, int &range_max);
    int getLargestCommandRange(int stype, String action, String meaning);
    bool isCommandValidIn(ValueTree command, int stype);
    
    ValueTree wantAction(String action, int stype);
    void wantProperty(ValueTree want, String meaning, int value);
    ValueTree createCommand(ValueTree want, bool warnIfImpossible = true);
    void advanceToTimestamp(ValueTree section, int stype, int &cmd, int &t, int newt);
    ValueTree createMarker();
    int getNewCommandLength(ValueTree command); //For a command that was just created, not one read from an existing sequence
    
    bool isCloseEnough(ValueTree command1, ValueTree command2, bool allowCCMerge, ValueTree midiopts);
    int getTotalSectionTime(ValueTree section);
    void deleteSection(int sectodelete);
    void getExtendedCC(MidiMessage msg, int &cc, int &value);
    
    void optimize(ValueTree midiopts);
    void reduceTrackNotes();
    
    //For exportMus
    
    void assignTSection(ValueTree sec, int tsecnum);
    int countTicks(ValueTree sec);
    String getSecNamePrefix(int dialect, ValueTree parent);
    
    //For importCom
    ValueTree getDescription(uint8_t firstbyte, int stype); //Stype: 0 seq hdr, 1 chn hdr, 2 track data
    ValueTree getCommand(Array<uint8_t> &data, uint32_t address, int stype);
    ValueTree initCommand(uint32_t address);
    ValueTree getDynTableCommand(Array<uint8_t> &data, uint32_t address, ValueTree section);
    ValueTree getEnvelopeCommand(Array<uint8_t> &data, uint32_t address);
    ValueTree getMessageCommand(Array<uint8_t> &data, uint32_t address, ValueTree section);
    ValueTree getOtherTableCommand(Array<uint8_t> &data, uint32_t address);
    
    //<0: error, 1: section escape, 2: restart_parsing
    int checkRanIntoOtherSection(int parse_stype, int &parse_s, uint32_t parse_addr, 
        ValueTree parse_cmd);
    //<0: error, 0: not found, 1: found
    int findTargetCommand(String action, uint32_t parse_addr, int tgt_addr, int tgt_stype, ValueTree parse_cmd);
    //<0: error, 0: okay
    int createSection(String src_action, int tgt_addr, int tgt_stype, ValueTree parse_cmd,
        ValueTree parse_section);
    
    int getPtrAddress(ValueTree command, uint32_t currentAddr, int seqlen);
    bool removeSection(int remove, int &replace, int hash, int cmdbyte);
    int actionTargetSType(String action, int stype, uint32_t a);
    void clearRecurVisited();
    bool findDynTableType(int dtsec);
    int findNextDynTableType(int s, int c);
    bool getSectionAndCmd(ValueTree command, int &s, int &c);
    
    struct SectionSorter {
        static int compareElements(const ValueTree &first, const ValueTree &second){
            int one = first.getProperty(idAddress, -1);
            int two = second.getProperty(idAddress, -1);
            jassert(one != two && one >= 0 && two >= 0);
            return one - two;
        }
    };

    //For exportCom
    void writeCommand(Array<uint8_t> &data, uint32_t address, ValueTree command);
    
    //Identifiers
    
    static Identifier idName;
    static Identifier idCName;
    static Identifier idOName;
    static Identifier idCmd;
    static Identifier idCmdEnd;
    static Identifier idAction;
    static Identifier idMeaning;
    static Identifier idValue;
    static Identifier idCC;
    static Identifier idLength;
    static Identifier idAddress;
    static Identifier idAddressEnd;
    static Identifier idDataSrc;
    static Identifier idDataLen;
    static Identifier idDataForce2;
    static Identifier idDataAddr;
    static Identifier idDataActualLen;
    static Identifier idSType;
    static Identifier idValidInSeq;
    static Identifier idValidInChn;
    static Identifier idValidInTrk;
    static Identifier idChannel;
    static Identifier idLayer;
    static Identifier idTSection;
    static Identifier idSection;
    static Identifier idSectionName;
    static Identifier idOldSectionIdx;
    static Identifier idSecDone;
    static Identifier idTicks;
    static Identifier idLabelName;
    static Identifier idLabelNameAuto;
    static Identifier idSrcCmdRef;
    static Identifier idHash;
    static Identifier idTargetSection;
    static Identifier idTargetHash;
    static Identifier idTargetCmdByte;
    static Identifier idWillDrop;
    static Identifier idDynTableSType;
    static Identifier idDynTableDynSType;
    //static Identifier idCurDynTableSec;
    static Identifier idMessage;
    static Identifier idRecurVisited;
    
};
