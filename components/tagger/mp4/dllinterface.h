 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#ifdef callbacks
#	undef callbacks
#endif

#include <mp4v2/mp4v2.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*mp4v2dll;

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef MP4FileHandle		(*MP4READ)			(const char *);
typedef MP4FileHandle		(*MP4MODIFY)			(const char *, uint32_t);
typedef void			(*MP4CLOSE)			(MP4FileHandle, uint32_t);
typedef void			(*MP4FREE)			(void *);
typedef bool			(*MP4OPTIMIZE)			(const char *, const char *);

typedef const MP4Tags *		(*MP4TAGSALLOC)			();
typedef bool			(*MP4TAGSFETCH)			(const MP4Tags *, MP4FileHandle);
typedef bool			(*MP4TAGSSTORE)			(const MP4Tags *, MP4FileHandle);
typedef void			(*MP4TAGSFREE)			(const MP4Tags *);

typedef bool			(*MP4TAGSSETNAME)		(const MP4Tags *, const char *);
typedef bool			(*MP4TAGSSETARTIST)		(const MP4Tags *, const char *);
typedef bool			(*MP4TAGSSETALBUM)		(const MP4Tags *, const char *);
typedef bool			(*MP4TAGSSETCOMMENTS)		(const MP4Tags *, const char *);
typedef bool			(*MP4TAGSSETGENRE)		(const MP4Tags *, const char *);
typedef bool			(*MP4TAGSSETGENRETYPE)		(const MP4Tags *, const uint16_t *);
typedef bool			(*MP4TAGSSETRELEASEDATE)	(const MP4Tags *, const char *);
typedef bool			(*MP4TAGSSETTRACK)		(const MP4Tags *, const MP4TagTrack *);
typedef bool			(*MP4TAGSSETDISK)		(const MP4Tags *, const MP4TagDisk *);
typedef bool			(*MP4TAGSSETCOMPILATION)	(const MP4Tags *, const uint8_t *);
typedef bool			(*MP4TAGSSETMEDIATYPE)		(const MP4Tags *, const uint8_t *);
typedef bool			(*MP4TAGSSETENCODINGTOOL)	(const MP4Tags *, const char *);

typedef bool			(*MP4TAGSADDARTWORK)		(const MP4Tags *, MP4TagArtwork *);
typedef bool			(*MP4TAGSREMOVEARTWORK)		(const MP4Tags *, uint32_t);

typedef MP4ChapterType		(*MP4GETCHAPTERS)		(MP4FileHandle, MP4Chapter_t **, uint32_t *, MP4ChapterType);
typedef MP4ChapterType		(*MP4SETCHAPTERS)		(MP4FileHandle, MP4Chapter_t *, uint32_t, MP4ChapterType);
typedef MP4ChapterType		(*MP4DELETECHAPTERS)		(MP4FileHandle, MP4ChapterType, MP4TrackId);

typedef MP4ItmfItem *		(*MP4ITMFITEMALLOC)		(const char *, uint32_t);
typedef void			(*MP4ITMFITEMFREE)		(MP4ItmfItem *);
typedef bool			(*MP4ITMFADDITEM)		(MP4FileHandle, const MP4ItmfItem *);
typedef bool			(*MP4ITMFREMOVEITEM)		(MP4FileHandle, const MP4ItmfItem *);
typedef MP4ItmfItemList *	(*MP4ITMFGETITEMS)		(MP4FileHandle);
typedef MP4ItmfItemList *	(*MP4ITMFGETITEMSBYCODE)	(MP4FileHandle, const char *);
typedef MP4ItmfItemList *	(*MP4ITMFGETITEMSBYMEANING)	(MP4FileHandle, const char *, const char *);
typedef void			(*MP4ITMFITEMLISTFREE)		(MP4ItmfItemList *);

extern MP4READ			 ex_MP4Read;
extern MP4MODIFY		 ex_MP4Modify;
extern MP4CLOSE			 ex_MP4Close;
extern MP4FREE			 ex_MP4Free;
extern MP4OPTIMIZE		 ex_MP4Optimize;

extern MP4TAGSALLOC		 ex_MP4TagsAlloc;
extern MP4TAGSFETCH		 ex_MP4TagsFetch;
extern MP4TAGSSTORE		 ex_MP4TagsStore;
extern MP4TAGSFREE		 ex_MP4TagsFree;

extern MP4TAGSSETNAME		 ex_MP4TagsSetName;
extern MP4TAGSSETARTIST		 ex_MP4TagsSetArtist;
extern MP4TAGSSETALBUM		 ex_MP4TagsSetAlbum;
extern MP4TAGSSETCOMMENTS	 ex_MP4TagsSetComments;
extern MP4TAGSSETGENRE		 ex_MP4TagsSetGenre;
extern MP4TAGSSETGENRETYPE	 ex_MP4TagsSetGenreType;
extern MP4TAGSSETRELEASEDATE	 ex_MP4TagsSetReleaseDate;
extern MP4TAGSSETTRACK		 ex_MP4TagsSetTrack;
extern MP4TAGSSETDISK		 ex_MP4TagsSetDisk;
extern MP4TAGSSETCOMPILATION	 ex_MP4TagsSetCompilation;
extern MP4TAGSSETMEDIATYPE	 ex_MP4TagsSetMediaType;
extern MP4TAGSSETENCODINGTOOL	 ex_MP4TagsSetEncodingTool;

extern MP4TAGSADDARTWORK	 ex_MP4TagsAddArtwork;
extern MP4TAGSREMOVEARTWORK	 ex_MP4TagsRemoveArtwork;

extern MP4GETCHAPTERS		 ex_MP4GetChapters;
extern MP4SETCHAPTERS		 ex_MP4SetChapters;
extern MP4DELETECHAPTERS	 ex_MP4DeleteChapters;

extern MP4ITMFITEMALLOC		 ex_MP4ItmfItemAlloc;
extern MP4ITMFITEMFREE		 ex_MP4ItmfItemFree;
extern MP4ITMFADDITEM		 ex_MP4ItmfAddItem;
extern MP4ITMFREMOVEITEM	 ex_MP4ItmfRemoveItem;
extern MP4ITMFGETITEMS		 ex_MP4ItmfGetItems;
extern MP4ITMFGETITEMSBYCODE	 ex_MP4ItmfGetItemsByCode;
extern MP4ITMFGETITEMSBYMEANING	 ex_MP4ItmfGetItemsByMeaning;
extern MP4ITMFITEMLISTFREE	 ex_MP4ItmfItemListFree;
