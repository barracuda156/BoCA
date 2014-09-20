/* (c)Copyright 1996-2000 NTT Cyber Space Laboratories */
/*                Modified on 2000.09.06 by N. Iwakami */

#include "ChunkHelper.h"
#include <iostream>
#include <string.h>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// ヘッダチャンク作成の補助関数（チャンク情報ファイルからサブチャンクを作成する）
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// TRAC 型
static
int ReadTracChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk )
{
	// 必要なデータの読み込み
	int trackNumber=0;

	if ( fscanf( fp, "%d", &trackNumber ) == 1 ) {
		// サブチャンクを作成・追加
		CTracChunk subChunk( id, trackNumber );
		theChunkChunk->PutChunk( subChunk );
	}

	return 0;
}

// YEAR 型
static
int ReadYearChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk )
{
	// 必要なデータの読み込み
	int year = 0;
	int month = 0;

	if ( fscanf( fp, "%d %d", &year, &month ) == 2 ) {
		// サブチャンクを作成・追加
		CYearChunk subChunk( id, year, month );
		theChunkChunk->PutChunk( subChunk );
	}

	return 0;
}

// ENCD 型
static
int ReadEncdChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk )
{
	// 必要なデータの読み込み
	int year=0, month=0, day=0, hour=0, minute=0, timeZone=0;

	if ( fscanf( fp, "%d %d %d %d %d %d", &year, &month, &day, &hour, &minute, &timeZone ) == 6 ) {
		// サブチャンクを作成・追加
		CEncdChunk subChunk( id, year, month, day, hour, minute, timeZone );
		theChunkChunk->PutChunk( subChunk );
	}

	return 0;
}

// 汎用型（文字列型を含む）
static
int ReadRawChunk( FILE *fp, std::string id, CChunkChunk *theChunkChunk ){
	// チャンク作成
	CChunk subChunk( id );

	// 必要なデータの読み込み
	char buf;
	while ( fread( &buf, 1, sizeof(char), fp ) ){
		subChunk.PutData( 1, &buf );
	}

	// サブチャンクを追加
	theChunkChunk->PutChunk( subChunk );

	return 0;
}



//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// チャンク読み込みテーブル
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
#define ReadStringChunk ReadRawChunk

typedef struct {
	char* id;
	int (*reader)( FILE *fp, std::string id, CChunkChunk *theChunkChunk );
} CHUNK_READ_TABLE;

static CHUNK_READ_TABLE primaryReadTable[] = {
	{ (char *) "TRAC", ReadTracChunk },	// トラック番号
	{ (char *) "YEAR", ReadYearChunk },	// 録音された年
	{ (char *) "ENCD", ReadEncdChunk },	// エンコードの日付
	{ (char *) "GUID", ReadRawChunk },	// Globally Unique Identifier
	{ (char *) "ISRC", ReadRawChunk },	// International Standard Record Code
	{ (char *) "ALBM", ReadStringChunk },	// アルバムタイトル
	{ (char *) "LYRC", ReadStringChunk },	// 歌詞
	{ (char *) "WORD", ReadStringChunk },	// 作詞者
	{ (char *) "MUSC", ReadStringChunk },	// 作曲者
	{ (char *) "ARNG", ReadStringChunk },	// 編曲者
	{ (char *) "PROD", ReadStringChunk },	// プロデューサー
	{ (char *) "REMX", ReadStringChunk },	// リミックス
	{ (char *) "CDCT", ReadStringChunk },	// 指揮者
	{ (char *) "SING", ReadStringChunk },	// 歌手
	{ (char *) "LABL", ReadStringChunk },	// レーベル
	{ (char *) "NOTE", ReadStringChunk },	// ライナーノーツ
	{ (char *) "PRSN", ReadStringChunk },	// 演奏メンバー
	{ (char *) "BAND", ReadStringChunk },	// バンド名
	{ NULL },
};

static CHUNK_READ_TABLE secondaryReadTable[] = {
	{ (char *) "NAME", ReadStringChunk },	// 曲名
	{ (char *) "AUTH", ReadStringChunk },	// アーティスト
	{ (char *) "COMT", ReadStringChunk },	// コメント
	{ (char *) "(c) ", ReadStringChunk },	// 著作権

	{ (char *) "ALBM", ReadStringChunk },	// アルバムタイトル
	{ (char *) "LYRC", ReadStringChunk },	// 歌詞
	{ (char *) "WORD", ReadStringChunk },	// 作詞者
	{ (char *) "MUSC", ReadStringChunk },	// 作曲者
	{ (char *) "ARNG", ReadStringChunk },	// 編曲者
	{ (char *) "PROD", ReadStringChunk },	// プロデューサー
	{ (char *) "REMX", ReadStringChunk },	// リミックス
	{ (char *) "CDCT", ReadStringChunk },	// 指揮者
	{ (char *) "SING", ReadStringChunk },	// 歌手
	{ (char *) "LABL", ReadStringChunk },	// レーベル
	{ (char *) "NOTE", ReadStringChunk },	// ライナーノーツ
	{ (char *) "PRSN", ReadStringChunk },	// 演奏メンバー
	{ (char *) "BAND", ReadStringChunk },	// バンド名
	{ NULL },
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// ヘッダチャンク作成関数
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
/*----------------------------------------------------------------------------*/
/* Name:        PutTextChunk()                                                */
/* Description: 文字列チャンクを作成して TWIN チャンクに付け加える            */
/* Return:      なし                                                          */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
void PutTextChunk( char* id, char* theData, CChunkChunk* twinChunk ) {
	if ( strlen( theData ) ) {
		CStringChunk theStringChunk( id, theData );
		twinChunk->PutChunk( theStringChunk );
	}
}

/*----------------------------------------------------------------------------*/
/* Name:        PutStandardChunks()                                           */
/* Description: TWIN チャンクに標準チャンクを付け加える                       */
/* Return:      (int) エラーがあれば１、なければ０                            */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int PutStandardChunks( headerInfo* info, CChunkChunk* twinChunk )
{
	// COMM チャンクを作成して TWIN チャンクに付け加える
	std::string version = twinChunk->GetID();
	CCommChunk theCommChunk( info->channelMode, info->bitRate, info->samplingRate, info->securityLevel );
	twinChunk->PutChunk( theCommChunk );

	// 標準文字列チャンク情報を作成して TWIN チャンクに付け加える
	PutTextChunk( (char *) "NAME", info->Name, twinChunk );
	PutTextChunk( (char *) "COMT", info->Comt, twinChunk );
	PutTextChunk( (char *) "AUTH", info->Auth, twinChunk );
	PutTextChunk( (char *) "(c) ", info->Cpyr, twinChunk );
	PutTextChunk( (char *) "FILE", info->File, twinChunk );

	// データサイズチャンクを作成して付け加える
	if ( info->Dsiz > 0 ) {
		CDsizChunk dsizChunk( "DSIZ", info->Dsiz );
		twinChunk->PutChunk( dsizChunk );
	}

	return 0;
}

/*----------------------------------------------------------------------------*/
/* Name:        PutChunkInfo()                                                */
/* Description: チャンク情報をファイルから読み込み twin チャンクに追加する    */
/* Return:      (int) エラーがあれば１、なければ０                            */
/* Access:      static                                                        */
/*----------------------------------------------------------------------------*/
static
int PutChunkInfo ( CHUNK_READ_TABLE theTable[], CChunkChunk *twinChunk, int mode, std::string directory )
{
	std::string infoDir[] = { "/", "/SCND/" };

	if ( mode >= 2 ) return 1;

	while ( theTable->id ) {
		/* ファイル名を作る */
		std::string fileName = theTable->id;
		std::string pathName = directory + infoDir[mode] + fileName;
		
		/* ファイル名をチェックし、ファイルがあれば情報を取り込む */
		FILE *fp;
		if ( (fp=fopen(pathName.c_str(), "rb")) ) {
			(*theTable->reader)( fp, theTable->id, twinChunk );
			fclose ( fp );
		}
		++theTable;
	}

	return 0;
}

/*============================================================================*/
/* Name:        TvqCreateHeaderChunk()                                        */
/* Description: ヘッダチャンクを作成する。                                    */
/* Return:      ヘッダチャンク。作成に失敗した場合 NULL を戻す                */
/* Access:      external                                                      */
/*============================================================================*/
CChunkChunk* TvqCreateHeaderChunk( headerInfo* setupInfo, std::string directory )
{
	/* Create empty "TWIN" chunk */
	CChunkChunk* twinChunk = new CChunkChunk( setupInfo->ID );

	/* Put standard chunks in the "TWIN" chunk */
	if ( PutStandardChunks( setupInfo, twinChunk ) ) return NULL;

	/* Put extended chunks (primary) in the "TWIN" chunk */
	if ( PutChunkInfo( primaryReadTable, twinChunk, PRIMARY, directory ) ) return NULL;

	/* Put extended chunks (secondary) in the "TWIN" chunk */
	{
		CChunkChunk scndChunk( "SCND" );
		if ( PutChunkInfo( secondaryReadTable, &scndChunk, SECONDARY, directory ) ) return NULL;
		if ( scndChunk.GetSize() ) {
			twinChunk->PutChunk( scndChunk );
		}
	}

	return twinChunk;
}
