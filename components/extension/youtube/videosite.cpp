 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "videosite.h"
#include "scriptutilities.h"

BoCA::VideoSite::VideoSite(const String &fileName)
{
	ParseXML(fileName);

	CreateScriptContext();
}

BoCA::VideoSite::~VideoSite()
{
	DestroyScriptContext();
}

Bool BoCA::VideoSite::IsSane()
{
	return !context.IsEmpty();
}

Bool BoCA::VideoSite::CanHandleURL(const String &URL)
{
	/* Lock the V8 engine to our thread.
	 */
	v8::Locker		 locker(isolate);
	v8::Isolate::Scope	 isolateScope(isolate);

	/* Use our own handle scope for this.
	 */
	v8::HandleScope		 handleScope(isolate);

	/* Enter the created context for calling the function.
	 */
	v8::Context::Scope	 contextScope(isolate, context);

	/* Get context from persistent handle.
	 */
	v8::Local<v8::Context>	 context = v8::Local<v8::Context>::New(isolate, this->context);

	/* Get function handle.
	 */
	v8::Handle<v8::String>	 func_name = v8::String::New("canHandleURL");
	v8::Handle<v8::Value>	 func_val = context->Global()->Get(func_name);

	v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(func_val);

	/* Set up arguments and call the function.
	 */
	const int		 argc = 1;
	v8::Handle<v8::Value>	 argv[argc] = { v8::String::New(URL) };

	v8::Handle<v8::Value>	 result = func->Call(context->Global(), argc, argv);

	return result->BooleanValue();
}

String BoCA::VideoSite::GetDecoderID(const String &html)
{
	if (decoders.Length() == 1) return decoders.GetFirst();

	/* Lock the V8 engine to our thread.
	 */
	v8::Locker		 locker(isolate);
	v8::Isolate::Scope	 isolateScope(isolate);

	/* Use our own handle scope for this.
	 */
	v8::HandleScope		 handleScope(isolate);

	/* Enter the created context for calling the function.
	 */
	v8::Context::Scope	 contextScope(isolate, context);

	/* Get context from persistent handle.
	 */
	v8::Local<v8::Context>	 context = v8::Local<v8::Context>::New(isolate, this->context);

	/* Get function handle.
	 */
	v8::Handle<v8::String>	 func_name = v8::String::New("getDecoderID");
	v8::Handle<v8::Value>	 func_val = context->Global()->Get(func_name);

	v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(func_val);

	/* Set up arguments and call the function.
	 */
	const int		 argc = 1;
	v8::Handle<v8::Value>	 argv[argc] = { v8::String::New(html.ConvertTo("ISO-8859-1")) };

	v8::Handle<v8::Value>	 result = func->Call(context->Global(), argc, argv);

	return (char *) *v8::String::AsciiValue(result);
}

String BoCA::VideoSite::GetVideoURL(const String &html)
{
	/* Lock the V8 engine to our thread.
	 */
	v8::Locker		 locker(isolate);
	v8::Isolate::Scope	 isolateScope(isolate);

	/* Use our own handle scope for this.
	 */
	v8::HandleScope		 handleScope(isolate);

	/* Enter the created context for calling the function.
	 */
	v8::Context::Scope	 contextScope(isolate, context);

	/* Get context from persistent handle.
	 */
	v8::Local<v8::Context>	 context = v8::Local<v8::Context>::New(isolate, this->context);

	/* Get function handle.
	 */
	v8::Handle<v8::String>	 func_name = v8::String::New("getVideoURL");
	v8::Handle<v8::Value>	 func_val = context->Global()->Get(func_name);

	v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(func_val);

	/* Set up arguments and call the function.
	 */
	const int		 argc = 1;
	v8::Handle<v8::Value>	 argv[argc] = { v8::String::New(html.ConvertTo("ISO-8859-1")) };

	v8::Handle<v8::Value>	 result = func->Call(context->Global(), argc, argv);

	return (char *) *v8::String::AsciiValue(result);
}

Metadata BoCA::VideoSite::QueryMetadata(const String &html)
{
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Extensions::Video Downloader");

	/* Lock the V8 engine to our thread.
	 */
	v8::Locker		 locker(isolate);
	v8::Isolate::Scope	 isolateScope(isolate);

	/* Use our own handle scope for this.
	 */
	v8::HandleScope		 handleScope(isolate);

	/* Enter the created context for calling the function.
	 */
	v8::Context::Scope	 contextScope(isolate, context);

	/* Get context from persistent handle.
	 */
	v8::Local<v8::Context>	 context = v8::Local<v8::Context>::New(isolate, this->context);

	/* Get function handle.
	 */
	v8::Handle<v8::String>	 func_name = v8::String::New("queryMetadata");
	v8::Handle<v8::Value>	 func_val = context->Global()->Get(func_name);

	v8::Handle<v8::Function> func = v8::Handle<v8::Function>::Cast(func_val);

	/* Set up arguments and call the function.
	 */
	const int		 argc = 1;
	v8::Handle<v8::Value>	 argv[argc] = { v8::String::New(html.ConvertTo("ISO-8859-1")) };

	v8::Handle<v8::Value>	 result = func->Call(context->Global(), argc, argv);
	v8::Handle<v8::Object>	 object = result->ToObject();

	/* Extract data from return value.
	 */
	Metadata	 metadata;
	String		 previous = String::SetInputFormat("UTF-8");

	metadata.title	     = object->Get(v8::String::New("title"))->IsString()	? ReplaceInnerHTML((char *) *v8::String::Utf8Value(object->Get(v8::String::New("title"))))	 : String(i18n->TranslateString("unknown title"));
	metadata.description = object->Get(v8::String::New("description"))->IsString()	? ReplaceInnerHTML((char *) *v8::String::Utf8Value(object->Get(v8::String::New("description")))) : String();

	metadata.date	     = object->Get(v8::String::New("date"))->IsString()		? ReplaceInnerHTML((char *) *v8::String::Utf8Value(object->Get(v8::String::New("date"))))	 : String(i18n->TranslateString("unknown"));
	metadata.thumbnail   = object->Get(v8::String::New("thumbnail"))->IsString()	? ReplaceInnerHTML((char *) *v8::String::Utf8Value(object->Get(v8::String::New("thumbnail"))))	 : String();

	metadata.uploader    = object->Get(v8::String::New("uploader"))->IsString()	? ReplaceInnerHTML((char *) *v8::String::Utf8Value(object->Get(v8::String::New("uploader"))))	 : String(i18n->TranslateString("unknown"));

	String::SetInputFormat(previous);

	return metadata;
}

Bool BoCA::VideoSite::CreateScriptContext()
{
#ifndef _M_X64
	/* Manually set stack limit to a random stack address to
	 * work around "maximum call stack size exceeded" error.
	 */
	v8::ResourceConstraints	 rc;

	rc.set_stack_limit((uint32_t *) (((uint32_t) (intptr_t) &rc) / 2));

	v8::SetResourceConstraints(&rc);
#endif

	/* Get current isolate.
	 */
	isolate = v8::Isolate::GetCurrent();

	/* Enter the created isolate.
	 */
	v8::Isolate::Scope	 isolateScope(isolate);

	/* Use our own handle scope for this.
	 */
	v8::HandleScope		 handleScope(isolate);

	/* Create a template for the global object.
	 */
	v8::Handle<v8::ObjectTemplate>	 global = v8::ObjectTemplate::New();

	/* Bind alert function to Alert callback.
	 */
	global->Set(v8::String::New("alert"), v8::FunctionTemplate::New(Alert));

	/* Bind downloadURL function to DownloadURL callback.
	 */
	global->Set(v8::String::New("downloadURL"), v8::FunctionTemplate::New(DownloadURL));

	/* Create and bind template for Metadata class.
	 */
	v8::Handle<v8::FunctionTemplate>	 metadata = v8::FunctionTemplate::New();

	metadata->PrototypeTemplate()->Set(v8::String::New("title"), v8::FunctionTemplate::New());
	metadata->PrototypeTemplate()->Set(v8::String::New("description"), v8::FunctionTemplate::New());

	metadata->PrototypeTemplate()->Set(v8::String::New("date"), v8::FunctionTemplate::New());
	metadata->PrototypeTemplate()->Set(v8::String::New("thumbnail"), v8::FunctionTemplate::New());

	metadata->PrototypeTemplate()->Set(v8::String::New("uploader"), v8::FunctionTemplate::New());

	global->Set(v8::String::New("Metadata"), metadata);

	/* Create a new context.
	 */
	context.Reset(isolate, v8::Context::New(isolate, NIL, global));

	if (context.IsEmpty()) return False;

	/* Enter the created context for compiling and running the script.
	 */
	v8::Context::Scope	 contextScope(isolate, context);

	/* Create a string containing the JavaScript source code.
	 */
	v8::Handle<v8::String>	 source = v8::String::New(script);
	v8::Handle<v8::String>	 file = v8::String::New(name);

	/* Compile and run the source code.
	 */
	v8::Script::Compile(source, file)->Run();

	return True;
}

Bool BoCA::VideoSite::DestroyScriptContext()
{
	/* Create a scope to enter the isolate and dispose the context.
	 */
	{
		/* Enter the isolate.
		 */
		v8::Isolate::Scope	 isolateScope(isolate);

		/* Dispose the persistent context.
		 */
		context.Dispose();
	}

	return True;
}

Int BoCA::VideoSite::ParseXML(const String &fileName)
{
	XML::Document	*document = new XML::Document();

	document->LoadFile(fileName);

	XML::Node	*root = document->GetRootNode();

	for (Int i = 0; i < root->GetNOfNodes(); i++)
	{
		XML::Node	*node = root->GetNthNode(i);

		if 	(node->GetName() == "name")	name	= node->GetContent();
		else if (node->GetName() == "version")	version	= node->GetContent();
		else if (node->GetName() == "decoders")
		{
			for (Int j = 0; j < node->GetNOfNodes(); j++)
			{
				XML::Node	*nodeDecoder = node->GetNthNode(j);

				decoders.Add(nodeDecoder->GetContent());
			}
		}
		else if (node->GetName() == "script")	script	= node->GetContent();
	}

	delete document;

	return Success();
}

/* Replaces inner HTML in a string.
 */
String BoCA::VideoSite::ReplaceInnerHTML(const String &text)
{
	String	 result = text.Trim();
	Int	 offset = -1;

	/* Set input format to ISO-8859-1.
	 */
	String	 prevInputFormat = String::SetInputFormat("ISO-8859-1");

	/* Remove multiple whitespaces.
	 */
	result.Replace("\n", " ");
	result.Replace("\r", " ");
	result.Replace("\t", " ");

	while (result.Find("  ") >= 0) result.Replace("  ", " ");

	/* Breaks
	 */
	result.Replace("<br/>", "\n");
	result.Replace("<br />", "\n");
	result.Replace("<br>", "\n");

	/* Bold
	 */
	result.Replace("<b>", NIL);
	result.Replace("</b>", NIL);

	/* Italic
	 */
	result.Replace("<i>", NIL);
	result.Replace("</i>", NIL);

	/* Emphasis
	 */
	result.Replace("<em>", NIL);
	result.Replace("</em>", NIL);

	/* Paragraph
	 */
	result.Replace("<p>", "\n");
	result.Replace("</p>", "\n");

	while ((offset = result.Find("<p ")) >= 0)
	{
		Int	 length = result.Tail(result.Length() - offset).Find(">");

									    result[offset] = '\n';
		for (Int i = offset + 1; i < result.Length() - length; i++) result[i]	   = result[i + length];

		result[result.Length() - length] = 0;
	}

	/* Link
	 */
	result.Replace("<a>", NIL);
	result.Replace("</a>", NIL);

	while ((offset = result.Find("<a ")) >= 0)
	{
		Int	 length = result.Tail(result.Length() - offset).Find(">") + 1;

		for (Int i = offset; i < result.Length() - length; i++) result[i] = result[i + length];

		result[result.Length() - length] = 0;
	}

	/* Ampersand
	 */
	result.Replace("&amp;", "&");
	result.Replace("&amp;", "&");

	/* Spaces
	 */
	result.Replace("&nbsp;", " ");
	result.Replace("&ensp;", " ");
	result.Replace("&emsp;", " ");
	result.Replace("&thinsp;", " ");

	/* Ellipse
	 */
	result.Replace("&hellip;", "...");

	/* Dashes
	 */
	result.Replace("&ndash;", "�");
	result.Replace("&mdash;", "�");

	/* Brackets
	 */
	result.Replace("&lt;", "<");
	result.Replace("&gt;", ">");

	/* Quotation
	 */
	result.Replace("&quot;", "\"");

	result.Replace("&bdquo;", "�");
	result.Replace("&ldquo;", "�");
	result.Replace("&rdquo;", "�");

	result.Replace("&sbquo;", "�");
	result.Replace("&lsquo;", "�");
	result.Replace("&rsquo;", "�");

	result.Replace("&laquo;", "�");
	result.Replace("&raquo;", "�");

	result.Replace("&lsaquo;", "�");
	result.Replace("&rsaquo;", "�");

	/* Umlaut
	 */
	result.Replace("&auml;", "�");
	result.Replace("&euml;", "�");
	result.Replace("&iuml;", "�");
	result.Replace("&ouml;", "�");
	result.Replace("&uuml;", "�");
	result.Replace("&yuml;", "�");

	result.Replace("&Auml;", "�");
	result.Replace("&Euml;", "�");
	result.Replace("&Iuml;", "�");
	result.Replace("&Ouml;", "�");
	result.Replace("&Uuml;", "�");

	/* Grave
	 */
	result.Replace("&agrave;", "�");
	result.Replace("&egrave;", "�");
	result.Replace("&igrave;", "�");
	result.Replace("&ograve;", "�");
	result.Replace("&ugrave;", "�");

	result.Replace("&Agrave;", "�");
	result.Replace("&Egrave;", "�");
	result.Replace("&Igrave;", "�");
	result.Replace("&Ograve;", "�");
	result.Replace("&Ugrave;", "�");

	/* Acute
	 */
	result.Replace("&aacute;", "�");
	result.Replace("&eacute;", "�");
	result.Replace("&iacute;", "�");
	result.Replace("&oacute;", "�");
	result.Replace("&uacute;", "�");
	result.Replace("&yacute;", "�");

	result.Replace("&Aacute;", "�");
	result.Replace("&Eacute;", "�");
	result.Replace("&Iacute;", "�");
	result.Replace("&Oacute;", "�");
	result.Replace("&Uacute;", "�");
	result.Replace("&Yacute;", "�");

	/* Circumflex
	 */
	result.Replace("&acirc;", "�");
	result.Replace("&ecirc;", "�");
	result.Replace("&icirc;", "�");
	result.Replace("&ocirc;", "�");
	result.Replace("&ucirc;", "�");

	result.Replace("&Acirc;", "�");
	result.Replace("&Ecirc;", "�");
	result.Replace("&Icirc;", "�");
	result.Replace("&Ocirc;", "�");
	result.Replace("&Ucirc;", "�");

	/* Tilde
	 */
	result.Replace("&atilde;", "�");
	result.Replace("&ntilde;", "�");
	result.Replace("&otilde;", "�");

	result.Replace("&Atilde;", "�");
	result.Replace("&Ntilde;", "�");
	result.Replace("&Otilde;", "�");

	/* Cedille
	 */
	result.Replace("&ccedil;", "�");
	result.Replace("&Ccedil;", "�");

	/* Slash
	 */
	result.Replace("&oslash;", "�");
	result.Replace("&Oslash;", "�");

	/* Ring
	 */
	result.Replace("&aring;", "�");
	result.Replace("&Aring;", "�");

	/* Nordic
	 */
	result.Replace("&eth;", "�");
	result.Replace("&thorn;", "�");

	result.Replace("&ETH;", "�");
	result.Replace("&THORN;", "�");

	/* Ligatures
	 */
	result.Replace("&aelig;", "�");
	result.Replace("&oelig;", "�");
	result.Replace("&szlig;", "�");

	result.Replace("&AElig;", "�");
	result.Replace("&OElig;", "�");

	/* Iberic
	 */
	result.Replace("&ordf;", "�");
	result.Replace("&ordm;", "�");

	result.Replace("&iexcl;", "�");
	result.Replace("&iquest;", "�");

	result.Replace("&middot;", "�");

	/* Currency
	 */
	result.Replace("&euro;", "�");
	result.Replace("&pound;", "�");

	/* Trade
	 */
	result.Replace("&copy;", "�");
	result.Replace("&reg;", "�");

	/* Mathematic
	 */
	result.Replace("&frac14;", "�");
	result.Replace("&frac12;", "�");
	result.Replace("&frac34;", "�");

	result.Replace("&times;", "�");
	result.Replace("&divide;", "�");

	/* Scientific
	 */
	result.Replace("&deg;", "�");
	result.Replace("&micro;", "�");

	/* Unicode
	 */
	while ((offset = result.Find("&#")) >= 0)
	{
		/* Entities
		 */
		Int	 value = result.SubString(offset + 2, 5).ToInt();
		Int	 length = result.SubString(offset + 2, 6).Find(";") + 2;

		result[offset] = value;

		for (Int i = offset + 1; i < result.Length() - length; i++) result[i] = result[i + length];

		result[result.Length() - length] = 0;
	}

	while ((offset = result.Find("\\u")) >= 0)
	{
		/* Escapes
		 */
		Int	 value = (Int64) Number::FromHexString(result.SubString(offset + 2, 4));

		result[offset] = value;

		for (Int i = offset + 1; i < result.Length() - 5; i++) result[i] = result[i + 5];

		result[result.Length() - 5] = 0;
	}

	/* Condense string.
	 */
	while (result.StartsWith(" ")  ||
	       result.StartsWith("\n") ||
	       result.StartsWith("\r") ||
	       result.StartsWith("\t")) result = result.Tail(result.Length() - 1);

	while (result.EndsWith(" ")  ||
	       result.EndsWith("\r") ||
	       result.EndsWith("\t")) result[result.Length() - 1] = 0;

	result.Replace("\n ", "\n");

	/* Restore previous input format.
	 */
	String::SetInputFormat(prevInputFormat);

	return result;
}
