#include "px.hpp"
#include "../px/file.hpp"
#include "../px/string.hpp"

#include <stdio.h>

#ifdef ANDROID
    #include <jni.h>
#endif

#define PX_MEMCPY(DST, SRC, SIZE) memcpy(DST, SRC, SIZE)
#define PX_PRINTF(DST, ...) printf(DST, __VA_ARGS__)
#define PX_SPRINTF(DST, SRC, ...) sprintf(DST, SRC, __VA_ARGS__)

namespace Px
{
	#ifdef PX_BUILD_API
		class ApiFunctions
		{
			public: chars Type;
			public: chars Name;
			public: chars Param;
			public: String Argument;
			public: ApiFunctions* Next;
			public: ApiFunctions(chars type, chars name, chars param)
				: Type(type), Name(name), Param(param)
			{
				if(param && CONST_STRING("void") != param)
				{
					sint08s collector;
					for(sint32 i = 0; Param[i]; ++i)
					{
						if(Param[i] == 'a' && Param[i + 1] == 'p' && Param[i + 2] == 'i' && Param[i + 3] == '_')
							while(Param[i++] != ' ');
						collector.AtAdding() = Param[i];
					}
					collector.AtAdding() = '\0';
					Argument = &collector[0];
				}
				Next = nullptr;
			}
			public: ~ApiFunctions() {delete Next;}
			public: static ApiFunctions* Top()
			{static ApiFunctions _(nullptr, nullptr, nullptr); return &_;}
		};

		api_autorun CollectForPlatform(chars type, chars name, chars param)
		{
			ApiFunctions* CurFunction = ApiFunctions::Top();
			while(CurFunction->Next) CurFunction = CurFunction->Next;
			CurFunction->Next = new ApiFunctions(type, name, param);
			return 0;
		}

		void ReadyForPlatform(void* env)
		{
			#ifdef ANDROID
				static JavaVM* jvm = nullptr;
				((JNIEnv*) env)->GetJavaVM(&jvm);
				jvm->AttachCurrentThread((JNIEnv**) &env, 0);
			#endif
		}

		api_string ReturnForPlatform(void* env, chars data)
		{
			#ifdef ANDROID
				return ((JNIEnv*) env)->NewStringUTF(data);
			#else
				return data;
			#endif
		}

		api_floats ReturnForPlatform(void* env, const float* data, const sint32 len)
		{
			#ifdef ANDROID
				jfloatArray result = ((JNIEnv*) env)->NewFloatArray(len);
				PX_MEMCPY(result, data, sizeof(float) * len);
				return result;
			#else
				return data;
			#endif
		}

		api_results ResultsForPlatform(void* env, char* temp, uint32 a, uint32 b)
		{
			PX_SPRINTF(temp, "%u|%u", a, b);
			#ifdef ANDROID
				return ((JNIEnv*) env)->NewStringUTF(temp);
			#else
				return temp;
			#endif
		}

		api_results ResultsForPlatform(void* env, char* temp, float a, float b, float c)
		{
			PX_SPRINTF(temp, "%f|%f|%f", a, b, c);
			#ifdef ANDROID
				return ((JNIEnv*) env)->NewStringUTF(temp);
			#else
				return temp;
			#endif
		}

		api_results ResultsForPlatform(void* env, char* temp, float a, float b, float c, float d)
		{
			PX_SPRINTF(temp, "%f|%f|%f|%f", a, b, c, d);
			#ifdef ANDROID
				return ((JNIEnv*) env)->NewStringUTF(temp);
			#else
				return temp;
			#endif
		}

		#ifdef ANDROID
			StringForPlatform::StringForPlatform(void* env, api_string str)
				: Utf8(((JNIEnv*) env)->GetStringUTFChars((jstring) str, nullptr)),
				  Env(env), Str(str) {}
			StringForPlatform::~StringForPlatform()
			{((JNIEnv*) Env)->ReleaseStringUTFChars((jstring) Str, Utf8);}
			StringForPlatform::operator chars() const
			{return Utf8;}
		#else
			StringForPlatform::StringForPlatform(void* env, api_string str)
				: Utf8(nullptr), Env(env), Str(str) {}
			StringForPlatform::~StringForPlatform() {}
			StringForPlatform::operator chars() const
			{return Str;}
		#endif

		#ifdef ANDROID
			ByteArrayForPlatform::ByteArrayForPlatform(void* env, api_bytes brr)
				: Bytes((bytes) ((JNIEnv*) env)->GetByteArrayElements((jbyteArray) brr, nullptr)),
				  Env(env), Brr(brr) {}
			ByteArrayForPlatform::~ByteArrayForPlatform()
			{((JNIEnv*) Env)->ReleaseByteArrayElements((jbyteArray) Brr, (jbyte*) Bytes, JNI_ABORT);}
			ByteArrayForPlatform::operator bytes() const
			{return Bytes;}
		#else
			ByteArrayForPlatform::ByteArrayForPlatform(void* env, api_bytes brr)
				: Bytes(nullptr), Env(env), Brr(brr) {}
			ByteArrayForPlatform::~ByteArrayForPlatform() {}
			ByteArrayForPlatform::operator bytes() const
			{return Brr;}
		#endif

		class ApiBuilder
		{
			private: void* FilePtr;
			public: ApiBuilder(void* fileptr) : FilePtr(fileptr) {}
			public: ~ApiBuilder() {}
			public: void WriteTypes(chars value)
			{
				char Temp[1024];
				PX_SPRINTF(Temp,
					"#pragma once\r\n"
					"\r\n"
					"////////////////////////////////////////////////////////////////////////////////\r\n"
					"typedef int                    api_int;\r\n"
					"typedef unsigned int           api_uint;\r\n"
					"typedef int                    api_flag;\r\n"
					"typedef int                    api_enum;\r\n"
					"typedef long long              api_long;\r\n"
					"typedef float                  api_float;\r\n"
					"typedef const char*            api_string;\r\n"
					"typedef const unsigned char*   api_bytes;\r\n"
					"typedef const float*           api_floats;\r\n"
					"typedef const char*            api_results;\r\n"
					"\r\n"
					"////////////////////////////////////////////////////////////////////////////////\r\n"
					"void Init" PX_BUILD_API_NAME "(%s dll);\r\n", value);
				String Text = Temp;
				File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
			}
			public: void WriteDeclares()
			{
				ApiFunctions* CurFunction = ApiFunctions::Top();
				String LastGroupName = "";
				while(CurFunction = CurFunction->Next)
				{
					sint32 GroupNameLength = 0;
					for(int i = 0; CurFunction->Name[i]; ++i)
						if(CurFunction->Name[i] == '_')
							GroupNameLength = i;
					String CurGroupName(CurFunction->Name, GroupNameLength);
					if(LastGroupName.Compare(CurGroupName, 0) != 0)
					{
						LastGroupName = CurGroupName;
						String Text =
							"\r\n"
							"////////////////////////////////////////////////////////////////////////////////\r\n";
						File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
					}

					char Temp[1024];
					PX_SPRINTF(Temp, "%s %s(%s);\r\n",
						CurFunction->Type, CurFunction->Name, CurFunction->Param);
					String Text = Temp;
					File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				}
			}
			public: void WriteIncludes(chars value)
			{
				char Temp[1024];
				PX_SPRINTF(Temp,
					"#include %s\r\n"
					"#include \"" PX_BUILD_API_NAME ".h\"\r\n", value);
				String Text = Temp;
				File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
			}
			public: void WriteTypedefs()
			{
				String Text =
					"\r\n"
					"////////////////////////////////////////////////////////////////////////////////\r\n";
				File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				ApiFunctions* CurFunction = ApiFunctions::Top();
				while(CurFunction = CurFunction->Next)
				{
					char Temp[1024];
					PX_SPRINTF(Temp,
						"extern \"C\" typedef %s (*_%s)(%s);\r\n"
						"static _%s DLL_%s = nullptr;\r\n",
						CurFunction->Type, CurFunction->Name, CurFunction->Param,
						CurFunction->Name, CurFunction->Name);
					Text = Temp;
					File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				}
			}
			public: void WriteInitializer(chars value1, chars value2)
			{
				char Temp[1024];
				PX_SPRINTF(Temp,
					"\r\n"
					"////////////////////////////////////////////////////////////////////////////////\r\n"
					"void Init" PX_BUILD_API_NAME "(%s dll)\r\n"
					"{\r\n", value1);
				String Text = Temp;
				File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				ApiFunctions* CurFunction = ApiFunctions::Top();
				while(CurFunction = CurFunction->Next)
				{
					PX_SPRINTF(Temp,
						"\tDLL_%s = (_%s) %s(dll, \"%s\");\r\n",
						CurFunction->Name, CurFunction->Name, value2, CurFunction->Name);
					Text = Temp;
					File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				}
				Text = "}\r\n";
				File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
			}
			public: void WriteImplement()
			{
				String Text = "\r\n"
					"////////////////////////////////////////////////////////////////////////////////";
				File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				ApiFunctions* CurFunction = ApiFunctions::Top();
				while(CurFunction = CurFunction->Next)
				{
					char Temp[1024];
					PX_SPRINTF(Temp,
						"\r\n"
						"%s %s(%s)\r\n"
						"{\r\n"
						"\t%sDLL_%s(%s);\r\n"
						"}\r\n",
						CurFunction->Type, CurFunction->Name, CurFunction->Param,
						(CONST_STRING("void") == CurFunction->Type)? "" : "return ",
						CurFunction->Name, (chars) CurFunction->Argument);
					Text = Temp;
					File::Write(FilePtr, (bytes)(chars) Text, Text.Length());
				}
			}
		};

		PX_API_FUNC(void, Px_PrintFunctions)()
		{
			ApiFunctions* CurFunction = ApiFunctions::Top();
			for(sint32 i = 0; CurFunction = CurFunction->Next; ++i)
				PX_PRINTF("%03d> %s %s(%s)\r\n", i,
					CurFunction->Type, CurFunction->Name, CurFunction->Param);
		}

		PX_API_FUNC(void, Px_BuildForMFC)(chars platform)
		{
			void* HeaderFile = File::OpenForWrite(PX_BUILD_API_NAME ".h");
			ApiBuilder(HeaderFile).WriteTypes("HMODULE");
			ApiBuilder(HeaderFile).WriteDeclares();
			File::Close(HeaderFile);

			void* SourceFile = File::OpenForWrite(PX_BUILD_API_NAME ".cpp");
			ApiBuilder(SourceFile).WriteIncludes("\"stdafx.h\"");
			ApiBuilder(SourceFile).WriteTypedefs();
			ApiBuilder(SourceFile).WriteInitializer("HMODULE", "GetProcAddress");
			ApiBuilder(SourceFile).WriteImplement();
			File::Close(SourceFile);
		}

		PX_API_FUNC(void, Px_BuildForXCODE)(chars platform)
		{
			void* HeaderFile = File::OpenForWrite(PX_BUILD_API_NAME ".h");
			ApiBuilder(HeaderFile).WriteTypes("void*");
			ApiBuilder(HeaderFile).WriteDeclares();
			File::Close(HeaderFile);

			void* SourceFile = File::OpenForWrite(PX_BUILD_API_NAME ".mm");
			ApiBuilder(SourceFile).WriteIncludes("<dlfcn.h>");
			ApiBuilder(SourceFile).WriteTypedefs();
			ApiBuilder(SourceFile).WriteInitializer("void*", "dlsym");
			ApiBuilder(SourceFile).WriteImplement();
			File::Close(SourceFile);
		}

		PX_API_FUNC(void, Px_BuildForJAVA)()
		{
		}
	#endif
}
