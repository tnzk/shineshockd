#ifndef TTGSUBTable_H
#define TTGSUBTable_H

// Use FreeType2 library.
#include <ft2build.h>
#include FT_FREETYPE_H

#include "common.h"

class CTTGSUBTable{
public:
	CTTGSUBTable(void):loaded(false){};
	CTTGSUBTable(FT_Bytes gsub):loaded(false){
		LoadGSUBTable(gsub);
	}
	virtual ~CTTGSUBTable(){}

	bool IsOk(void) const{return loaded;}
	bool LoadGSUBTable(FT_Bytes gsub);

	bool GetVerticalGlyph(uint32_t glyphnum, uint32_t *vglyphnum);
	// メンバ構造体
	struct tt_gsub_header{
		uint32_t Version;
		uint16_t ScriptList;
		uint16_t FeatureList;
		uint16_t LookupList;
	};
	// ScriptList 用
	struct TLangSys{
		uint16_t LookupOrder;	// NULL
		uint16_t ReqFeatureIndex;	// if no required features = 0xFFFF
		uint16_t FeatureCount;
		uint16_t *FeatureIndex;	// Array

		TLangSys():LookupOrder(0),ReqFeatureIndex(0),FeatureCount(0),FeatureIndex(NULL){}
		~TLangSys(){if(FeatureIndex) delete[] FeatureIndex;}
	private:
		// コピー殺し
		TLangSys(const TLangSys&);
		TLangSys& operator=(const TLangSys&);
	};
	struct TLangSysRecord{
		uint32_t LangSysTag;
		struct TLangSys LangSys;

		TLangSysRecord():LangSysTag(0){}
	private:
		// コピー殺し
		TLangSysRecord(const TLangSysRecord&);
		TLangSysRecord& operator=(const TLangSysRecord&);
	};
	struct TScript{
		uint16_t DefaultLangSys;	// May-be NULL?
		uint16_t LangSysCount;
		struct TLangSysRecord *LangSysRecord;	// Array

		TScript():DefaultLangSys(0),LangSysCount(0),LangSysRecord(NULL){}
		~TScript(){if(LangSysRecord) delete[] LangSysRecord;}
	private:
		// コピー殺し
		TScript(const TScript&);
		TScript& operator=(const TScript&);
	};
	struct TScriptRecord{
		uint32_t ScriptTag;
		struct TScript Script;

		TScriptRecord():ScriptTag(0){}
	private:
		// コピー殺し
		TScriptRecord(const TScriptRecord&);
		TScriptRecord& operator=(const TScriptRecord&);
	};
	struct TScriptList{
		uint16_t ScriptCount;
		struct TScriptRecord *ScriptRecord;	// Array

		TScriptList():ScriptCount(0),ScriptRecord(NULL){}
		~TScriptList(){if(ScriptRecord) delete[] ScriptRecord;}
	private:
		// コピー殺し
		TScriptList(const TScriptList&);
		TScriptList& operator=(const TScriptList&);
	};

	// FeatureList 用
	struct TFeature{
		uint16_t FeatureParams;
		int LookupCount;
		uint16_t *LookupListIndex;	// Array

		TFeature():FeatureParams(0),LookupCount(0),LookupListIndex(NULL){}
		~TFeature(){if(LookupListIndex) delete[] LookupListIndex;}
	private:
		// コピー殺し
		TFeature(const TFeature&);
		TFeature& operator=(const TFeature&);
	};
	struct TFeatureRecord{
		uint32_t FeatureTag;
		struct TFeature Feature;

		TFeatureRecord():FeatureTag(0){}
	private:
		// コピー殺し
		TFeatureRecord(const TFeatureRecord&);
		TFeatureRecord& operator=(const TFeatureRecord&);
	};
	struct TFeatureList{
		int FeatureCount;
		struct TFeatureRecord *FeatureRecord;	// Array

		TFeatureList():FeatureCount(0),FeatureRecord(NULL){}
		~TFeatureList(){if(FeatureRecord) delete[] FeatureRecord;}
	private:
		// コピー殺し
		TFeatureList(const TFeatureList&);
		TFeatureList& operator=(const TFeatureList&);
	};

	// LookupList 用
	enum TLookupFlag{
		LOOKUPFLAG_RightToLeft = 0x0001,
		LOOKUPFLAG_IgnoreBaseGlyphs = 0x0002,
		LOOKUPFLAG_IgnoreLigatures = 0x0004,
		LOOKUPFLAG_IgnoreMarks = 0x0008,
		LOOKUPFLAG_Reserved = 0x00F0,
		LOOKUPFLAG_MarkAttachmentType = 0xFF00,
	};
	struct TCoverageFormatBase{
		uint16_t CoverageFormat;

		TCoverageFormatBase():CoverageFormat(0){}
		virtual ~TCoverageFormatBase(){}
	private:
		// コピー殺し
		TCoverageFormatBase(const TCoverageFormatBase&);
		TCoverageFormatBase& operator=(const TCoverageFormatBase&);
	};
	struct TCoverageFormat1: public TCoverageFormatBase{
		uint16_t GlyphCount;
		uint16_t *GlyphArray;	// Array

		TCoverageFormat1():GlyphCount(0),GlyphArray(NULL){CoverageFormat = 1;}
		~TCoverageFormat1(){if(GlyphArray) delete[] GlyphArray;}
	private:
		// コピー殺し
		TCoverageFormat1(const TCoverageFormat1&);
		TCoverageFormat1& operator=(const TCoverageFormat1&);
	};
	struct TRangeRecord{
		uint16_t Start;
		uint16_t End;
		uint16_t StartCoverageIndex;

		TRangeRecord():Start(0),End(0),StartCoverageIndex(0){}
	private:
		// コピー殺し
		TRangeRecord(const TRangeRecord&);
		TRangeRecord& operator=(const TRangeRecord&);
	};
	struct TCoverageFormat2: public TCoverageFormatBase{
		uint16_t RangeCount;
		struct TRangeRecord *RangeRecord;	// Array

		TCoverageFormat2():RangeCount(0),RangeRecord(NULL){CoverageFormat = 2;}
		~TCoverageFormat2(){if(RangeRecord) delete[] RangeRecord;}
	private:
		// コピー殺し
		TCoverageFormat2(const TCoverageFormat2&);
		TCoverageFormat2& operator=(const TCoverageFormat2&);
	};
	struct TClassDefFormatBase{
		uint16_t ClassFormat;

		TClassDefFormatBase():ClassFormat(0){}
		virtual ~TClassDefFormatBase(){}
	private:
		// コピー殺し
		TClassDefFormatBase(const TClassDefFormatBase&);
		TClassDefFormatBase& operator=(const TClassDefFormatBase&);
	};
	struct TClassDefFormat1: public TClassDefFormatBase{
		uint16_t StartGlyph;
		uint16_t GlyphCount;
		uint16_t *ClassValueArray;	// Array

		TClassDefFormat1():StartGlyph(0),GlyphCount(0),ClassValueArray(NULL){ClassFormat = 1;}
		~TClassDefFormat1(){if(ClassValueArray) delete[] ClassValueArray;}
	private:
		// コピー殺し
		TClassDefFormat1(const TClassDefFormat1&);
		TClassDefFormat1& operator=(const TClassDefFormat1&);
	};
	struct TClassRangeRecord{
		uint16_t Start;
		uint16_t End;
		uint16_t Class;

		TClassRangeRecord():Start(0),End(0),Class(0){}
	private:
		// コピー殺し
		TClassRangeRecord(const TClassRangeRecord&);
		TClassRangeRecord& operator=(const TClassRangeRecord&);
	};
	struct TClassDefFormat2: public TClassDefFormatBase{
		uint16_t ClassRangeCount;
		struct TClassRangeRecord *ClassRangeRecord;	// Array

		TClassDefFormat2():ClassRangeCount(0),ClassRangeRecord(NULL){ClassFormat = 2;}
		~TClassDefFormat2(){if(ClassRangeRecord) delete[] ClassRangeRecord;}
	private:
		// コピー殺し
		TClassDefFormat2(const TClassDefFormat2&);
		TClassDefFormat2& operator=(const TClassDefFormat2&);
	};
	struct TDevice{
		uint16_t StartSize;
		uint16_t EndSize;
		uint16_t DeltaFormat;

		TDevice():StartSize(0),EndSize(0),DeltaFormat(0){}
	private:
		// コピー殺し
		TDevice(const TDevice&);
		TDevice& operator=(const TDevice&);
	};
	struct TSubTableBase{
		uint16_t SubstFormat;

		TSubTableBase():SubstFormat(0){}
		virtual ~TSubTableBase(){}
	private:
		// コピー殺し
		TSubTableBase(const TSubTableBase&);
		TSubTableBase& operator=(const TSubTableBase&);
	};
	// LookupType 1: Single Substitution Subtable
	struct TSingleSubstFormat1: public TSubTableBase{
		TCoverageFormatBase *Coverage;
		int16_t DeltaGlyphID;

		TSingleSubstFormat1():DeltaGlyphID(0),Coverage(NULL){SubstFormat = 1;}
		~TSingleSubstFormat1(){if(Coverage) delete Coverage;}
	private:
		// コピー殺し
		TSingleSubstFormat1(const TSingleSubstFormat1&);
		TSingleSubstFormat1& operator=(const TSingleSubstFormat1&);
	};
	struct TSingleSubstFormat2: public TSubTableBase{
		TCoverageFormatBase *Coverage;
		uint16_t GlyphCount;
		uint16_t *Substitute;	// Array

		TSingleSubstFormat2():Coverage(NULL),GlyphCount(0),Substitute(NULL){SubstFormat = 2;}
		~TSingleSubstFormat2(){if(Coverage) delete Coverage;if(Substitute) delete[] Substitute;}
	private:
		// コピー殺し
		TSingleSubstFormat2(const TSingleSubstFormat2&);
		TSingleSubstFormat2& operator=(const TSingleSubstFormat2&);
	};
	struct TLookup{
		uint16_t LookupType;
		uint16_t LookupFlag;
		uint16_t SubTableCount;
		struct TSubTableBase **SubTable;	// Array

		TLookup():LookupType(0),LookupFlag(0),SubTableCount(0),SubTable(NULL){}
		~TLookup(){
			if(SubTableCount > 0 && SubTable != NULL){
				for(int i = 0;i < SubTableCount;i++){
					delete SubTable[i];
				}
				delete[] SubTable;
			}
		}
	private:
		// コピー殺し
		TLookup(const TLookup&);
		TLookup& operator=(const TLookup&);
	};
	struct TLookupList{
		int LookupCount;
		struct TLookup *Lookup;	// Array

		TLookupList():LookupCount(0),Lookup(NULL){}
		~TLookupList(){if(Lookup) delete[] Lookup;}
	private:
		// コピー殺し
		TLookupList(const TLookupList&);
		TLookupList& operator=(const TLookupList&);
	};

	// メンバ関数
	bool Parse(
		FT_Bytes scriptlist,
		FT_Bytes featurelist,
		FT_Bytes lookuplist);
	void ParseScriptList(FT_Bytes raw, TScriptList *rec);
	void ParseScript(FT_Bytes raw, TScript *rec);
	void ParseLangSys(FT_Bytes raw, TLangSys *rec);

	void ParseFeatureList(FT_Bytes raw, TFeatureList *rec);
	void ParseFeature(FT_Bytes raw, TFeature *rec);

	void ParseLookupList(FT_Bytes raw, TLookupList *rec);
	void ParseLookup(FT_Bytes raw, TLookup *rec);

	void ParseCoverage(FT_Bytes raw, TCoverageFormatBase **rec);
	void ParseCoverageFormat1(FT_Bytes raw, TCoverageFormat1 *rec);
	void ParseCoverageFormat2(FT_Bytes raw, TCoverageFormat2 *rec);

	void ParseSingleSubst(FT_Bytes raw, TSubTableBase **rec);
	void ParseSingleSubstFormat1(FT_Bytes raw, TSingleSubstFormat1 *rec);
	void ParseSingleSubstFormat2(FT_Bytes raw, TSingleSubstFormat2 *rec);

	// こういう不毛な命名は好きじゃないのだが
	bool GetVerticalGlyphSub(
			uint32_t glyphnum,
			uint32_t *vglyphnum,
			struct TFeature *Feature);
	bool GetVerticalGlyphSub2(
			uint32_t glyphnum,
			uint32_t *vglyphnum,
			struct TLookup *Lookup);
	// if not found, this function returns -1.
	int GetCoverageIndex(struct TCoverageFormatBase *Coverage, uint32_t g);

	// データ採取用
	uint8_t GetUInt8(FT_Bytes& p) const{
		uint8_t ret = p[0];
		p += 1;
		return ret;
	}
	int16_t GetInt16(FT_Bytes& p) const{
		uint16_t ret = p[0] << 8 | p[1];
		p += 2;
		return *(int16_t*)&ret;
	}
	uint16_t GetUInt16(FT_Bytes& p) const{
		uint16_t ret = p[0] << 8 | p[1];
		p += 2;
		return ret;
	}
	int32_t GetInt32(FT_Bytes& p) const{
		uint32_t ret = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
		p += 4;
		return *(int32_t*)&ret;
	}
	uint32_t GetUInt32(FT_Bytes& p) const{
		uint32_t ret = p[0] << 24 | p[1] << 16 | p[2] << 8 | p[3];
		p += 4;
		return ret;
	}

	// メンバ変数
	bool loaded;
	struct tt_gsub_header header;
	struct TScriptList ScriptList;
	struct TFeatureList FeatureList;
	struct TLookupList LookupList;
};

#endif
