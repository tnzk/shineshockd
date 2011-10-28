#include "ttgsubtable.hpp"

bool CTTGSUBTable::LoadGSUBTable(FT_Bytes gsub){
	header.Version = gsub[0] << 24 | gsub[1] << 16 | gsub[2] << 8 | gsub[3];
	if(header.Version != 0x00010000){
		return false;
	}
	header.ScriptList  = gsub[4] << 8 | gsub[5];
	header.FeatureList = gsub[6] << 8 | gsub[7];
	header.LookupList  = gsub[8] << 8 | gsub[9];
	return Parse(
		&gsub[header.ScriptList],
		&gsub[header.FeatureList],
		&gsub[header.LookupList]);
}

// ‚±‚ê‚ª‚È‚©‚È‚©‘å•Ï‚»‚¤‚¾‚ª
bool CTTGSUBTable::GetVerticalGlyph(uint32_t glyphnum, uint32_t *vglyphnum){
	// Feature‚©‚çTag‚ª'vrt2'‚©'vert'‚Ì‚à‚Ì‚ð’T‚µ‚Ä‚Ý‚é
	uint32_t tag[] = {
		(uint8_t)'v' << 24 |
		(uint8_t)'r' << 16 |
		(uint8_t)'t' <<  8 |
		(uint8_t)'2',

		(uint8_t)'v' << 24 |
		(uint8_t)'e' << 16 |
		(uint8_t)'r' <<  8 |
		(uint8_t)'t',
	};
	for(int i = 0;i < sizeof(tag) / sizeof(tag[0]);i++){
		for(int j = 0;j < FeatureList.FeatureCount;j++){
			if(FeatureList.FeatureRecord[j].FeatureTag == tag[i]
			&& GetVerticalGlyphSub(glyphnum,
				vglyphnum,
				&FeatureList.FeatureRecord[j].Feature)){
				return true;
			}
		}
	}
	// Œ©‚Â‚©‚ç‚È‚©‚Á‚½
	return false;
}

bool CTTGSUBTable::GetVerticalGlyphSub(
		uint32_t glyphnum,
		uint32_t *vglyphnum,
		struct TFeature *Feature){
	// Feature‚©‚çƒŠƒ“ƒN‚µ‚Ä‚¢‚éLookupTable‚ÌLookupType‚ª1(=Single)‚ð’T‚·
	for(int i = 0;i < Feature->LookupCount;i++){
		int index = Feature->LookupListIndex[i];
		if(index < 0 || LookupList.LookupCount < index){
			continue;
		}
		if(LookupList.Lookup[index].LookupType == 1){
			// Single
			if(GetVerticalGlyphSub2(
				glyphnum,
				vglyphnum,
				&LookupList.Lookup[index])){
				return true;
			}
		}
	}
	// Œ©‚Â‚©‚ç‚È‚©‚Á‚½
	return false;
}

bool CTTGSUBTable::GetVerticalGlyphSub2(
		uint32_t glyphnum,
		uint32_t *vglyphnum,
		struct TLookup *Lookup){
	// SubTable‚Ì’†‚©‚ç‰Æ‘{‚µ‚·‚é
	for(int i = 0;i < Lookup->SubTableCount;i++){
		switch(Lookup->SubTable[i]->SubstFormat){
		case 1:{
			TSingleSubstFormat1 *tbl1 = (TSingleSubstFormat1*)Lookup->SubTable[i];
			if(GetCoverageIndex(tbl1->Coverage, glyphnum) >= 0){
				*vglyphnum = glyphnum + tbl1->DeltaGlyphID;
				return true;
			}
			break;
		}
		case 2:{
			TSingleSubstFormat2 *tbl2 = (TSingleSubstFormat2*)Lookup->SubTable[i];
			int index = GetCoverageIndex(tbl2->Coverage, glyphnum);
			if(0 <= index && index < tbl2->GlyphCount){
				*vglyphnum = tbl2->Substitute[index];
				return true;
			}
			break;
		}
		}
	}
	return false;
}

int CTTGSUBTable::GetCoverageIndex(struct TCoverageFormatBase *Coverage, uint32_t g){
	int i;
	if(Coverage == NULL){
		return -1;
	}
	switch(Coverage->CoverageFormat){
	case 1:{
		TCoverageFormat1 *c1 = (TCoverageFormat1*)Coverage;
		for(i = 0;i < c1->GlyphCount;i++){
			if((uint32_t)c1->GlyphArray[i] == g){
				return i;	// Found.
			}
		}
		return -1;	// Not found.
	}
	case 2:{
		TCoverageFormat2 *c2 = (TCoverageFormat2*)Coverage;
		for(i = 0;i < c2->RangeCount;i++){
			uint32_t s = c2->RangeRecord[i].Start;
			uint32_t e = c2->RangeRecord[i].End;
			uint32_t si = c2->RangeRecord[i].StartCoverageIndex;
			if(si + s <= g && g <= si + e){
				return si + g - s;
			}
		}
		return -1;	// Not found.
	}
	}
	return -1;
}

bool CTTGSUBTable::Parse(
	FT_Bytes scriptlist,
	FT_Bytes featurelist,
	FT_Bytes lookuplist){
	ParseScriptList(scriptlist, &ScriptList);
	ParseFeatureList(featurelist, &FeatureList);
	ParseLookupList(lookuplist, &LookupList);
	return true;
}

void CTTGSUBTable::ParseScriptList(FT_Bytes raw, struct TScriptList *rec){
	int i;
	FT_Bytes sp = raw;
	rec->ScriptCount = GetUInt16(sp);
	if(rec->ScriptCount <= 0){
		return;	// –³‚¢
	}
	rec->ScriptRecord = new struct TScriptRecord[rec->ScriptCount];
	for(i = 0;i < rec->ScriptCount;i++){
		rec->ScriptRecord[i].ScriptTag = GetUInt32(sp);
		uint16_t offset = GetUInt16(sp);
		ParseScript(
			&raw[offset],
			&rec->ScriptRecord[i].Script);
	}
}

void CTTGSUBTable::ParseScript(FT_Bytes raw, struct TScript *rec){
	int i;
	FT_Bytes sp = raw;
	rec->DefaultLangSys = GetUInt16(sp);
	rec->LangSysCount = GetUInt16(sp);
	if(rec->LangSysCount <= 0){
		return;
	}
	rec->LangSysRecord = new struct TLangSysRecord[rec->LangSysCount];
	for(i = 0;i < rec->LangSysCount;i++){
		rec->LangSysRecord[i].LangSysTag = GetUInt32(sp);
		uint16_t offset = GetUInt16(sp);
		ParseLangSys(
			&raw[offset],
			&rec->LangSysRecord[i].LangSys);
	}
}

void CTTGSUBTable::ParseLangSys(FT_Bytes raw, struct TLangSys *rec){
	int i;
	FT_Bytes sp = raw;
	rec->LookupOrder = GetUInt16(sp);
	rec->ReqFeatureIndex = GetUInt16(sp);
	rec->FeatureCount = GetUInt16(sp);
	if(rec->FeatureCount <= 0)
		return;
	rec->FeatureIndex = new uint16_t[rec->FeatureCount];
}

void CTTGSUBTable::ParseFeatureList(FT_Bytes raw, TFeatureList *rec){
	int i;
	FT_Bytes sp = raw;
	rec->FeatureCount = GetUInt16(sp);
	if(rec->FeatureCount <= 0){
		return;
	}
	rec->FeatureRecord = new struct TFeatureRecord[rec->FeatureCount];
	for(i = 0;i < rec->FeatureCount;i++){
		rec->FeatureRecord[i].FeatureTag = GetUInt32(sp);
		uint16_t offset = GetUInt16(sp);
		ParseFeature(
			&raw[offset],
			&rec->FeatureRecord[i].Feature);
	}
}

void CTTGSUBTable::ParseFeature(FT_Bytes raw, TFeature *rec){
	int i;
	FT_Bytes sp = raw;
	rec->FeatureParams = GetUInt16(sp);
	rec->LookupCount = GetUInt16(sp);
	if(rec->LookupCount <= 0){
		return;
	}
	rec->LookupListIndex = new uint16_t[rec->LookupCount];
	for(i = 0;i < rec->LookupCount;i++){
		rec->LookupListIndex[i] = GetUInt16(sp);
	}
}

void CTTGSUBTable::ParseLookupList(FT_Bytes raw, TLookupList *rec){
	int i;
	FT_Bytes sp = raw;
	rec->LookupCount = GetUInt16(sp);
	if(rec->LookupCount <= 0){
		return;
	}
	rec->Lookup = new struct TLookup[rec->LookupCount];
	for(i = 0;i < rec->LookupCount;i++){
		uint16_t offset = GetUInt16(sp);
		ParseLookup(
			&raw[offset],
			&rec->Lookup[i]);
	}
}

void CTTGSUBTable::ParseLookup(FT_Bytes raw, TLookup *rec){
	int i;
	FT_Bytes sp = raw;
	rec->LookupType = GetUInt16(sp);
	rec->LookupFlag = GetUInt16(sp);
	rec->SubTableCount = GetUInt16(sp);
	if(rec->SubTableCount <= 0){
		return;
	}
	rec->SubTable = new struct TSubTableBase*[rec->SubTableCount];
	for(i = 0;i < rec->SubTableCount;i++){
		rec->SubTable[i] = NULL;	// ‚Æ‚è‚ ‚¦‚¸ˆÀ‘S‚Ì‚½‚ßNULL‚ð“ü‚ê‚Ä‚¨‚­
	}
	// ‚±‚¿‚ç‚Å‰ü‚ß‚Ä‚â‚éB
	// ‚Æ‚è‚ ‚¦‚¸LookupType‚ª1‚Ì‚Æ‚«‚¾‚¯‚Åc‘‚«‚Ío—ˆ‚é‚ñ‚¶‚á‚È‚©‚ë‚¤‚©B
	if(rec->LookupType != 1)
		return;
	for(i = 0;i < rec->SubTableCount;i++){
		uint16_t offset = GetUInt16(sp);
		ParseSingleSubst(
			&raw[offset],
			&rec->SubTable[i]);
	}
}

void CTTGSUBTable::ParseCoverage(FT_Bytes raw, TCoverageFormatBase **rec){
	FT_Bytes sp = raw;
	uint16_t Format = GetUInt16(sp);
	switch(Format){
	case 1:
		*rec = new TCoverageFormat1();
		ParseCoverageFormat1(raw, (TCoverageFormat1*)*rec);
		break;
	case 2:
		*rec = new TCoverageFormat2();
		ParseCoverageFormat2(raw, (TCoverageFormat2*)*rec);
		break;
	}
}
void CTTGSUBTable::ParseCoverageFormat1(FT_Bytes raw, TCoverageFormat1 *rec){
	int i;
	FT_Bytes sp = raw;
	GetUInt16(sp);	// “Ç‚ÝŽÌ‚Ä(Format)
	rec->GlyphCount = GetUInt16(sp);
	if(rec->GlyphCount <= 0){
		return;
	}
	rec->GlyphArray = new uint16_t[rec->GlyphCount];
	for(i = 0;i < rec->GlyphCount;i++){
		rec->GlyphArray[i] = GetUInt16(sp);
	}
}
void CTTGSUBTable::ParseCoverageFormat2(FT_Bytes raw, TCoverageFormat2 *rec){
	int i;
	FT_Bytes sp = raw;
	GetUInt16(sp);	// “Ç‚ÝŽÌ‚Ä(Format)
	rec->RangeCount = GetUInt16(sp);
	if(rec->RangeCount <= 0){
		return;
	}
	rec->RangeRecord = new TRangeRecord[rec->RangeCount];
	for(i = 0;i < rec->RangeCount;i++){
		rec->RangeRecord[i].Start = GetUInt16(sp);
		rec->RangeRecord[i].End = GetUInt16(sp);
		rec->RangeRecord[i].StartCoverageIndex = GetUInt16(sp);
	}
}

void CTTGSUBTable::ParseSingleSubst(FT_Bytes raw, TSubTableBase **rec){
	FT_Bytes sp = raw;
	uint16_t Format = GetUInt16(sp);
	switch(Format){
	case 1:
		*rec = new TSingleSubstFormat1();
		ParseSingleSubstFormat1(raw, (TSingleSubstFormat1*)*rec);
		break;
	case 2:
		*rec = new TSingleSubstFormat2();
		ParseSingleSubstFormat2(raw, (TSingleSubstFormat2*)*rec);
		break;
	}
}

void CTTGSUBTable::ParseSingleSubstFormat1(FT_Bytes raw, TSingleSubstFormat1 *rec){
	FT_Bytes sp = raw;
	GetUInt16(sp);	// “Ç‚ÝŽÌ‚Ä(Format)
	uint16_t offset = GetUInt16(sp);
	ParseCoverage(
		&raw[offset],
		&rec->Coverage);
	rec->DeltaGlyphID = GetInt16(sp);
}

void CTTGSUBTable::ParseSingleSubstFormat2(FT_Bytes raw, TSingleSubstFormat2 *rec){
	int i;
	FT_Bytes sp = raw;
	GetUInt16(sp);	// “Ç‚ÝŽÌ‚Ä(Format)
	uint16_t offset = GetUInt16(sp);
	ParseCoverage(
		&raw[offset],
		&rec->Coverage);
	rec->GlyphCount = GetUInt16(sp);
	if(rec->GlyphCount <= 0){
		return;
	}
	rec->Substitute = new uint16_t[rec->GlyphCount];
	for(i = 0;i < rec->GlyphCount;i++){
		rec->Substitute[i] = GetUInt16(sp);
	}
}

