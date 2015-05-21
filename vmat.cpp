#include <string.h>
#include <stdint.h>
#include <map>
#include <string>
#include <fstream>
#include <boost/assign.hpp>
#include "keyvalues.h"
#include "decompiler.h"
#include "vtex.h"

#include <iostream>

using std::ios;

std::map<std::string, std::string> szTexParamAliases =
boost::assign::map_list_of
	("g_tColor",              "TextureColor")
	("g_tColor0",             "TextureColor0")
	("g_tSpecular0",          "TextureReflectance0")
	("g_tSpecular1",          "TextureReflectance1")
	("g_tSpecular2",          "TextureReflectance2")
	("g_tSpecular3",          "TextureReflectance3")
	("g_tDetail",             "TextureDetail")
	("g_tDetail2",            "TextureDetail2")
	("g_tSpecularWarp",       "TextureSpecularWarp")
	("g_tDiffuseWarp",        "TextureDiffuseWarp")
	("g_tFresnelColorWarp3D", "TextureFresnelColorWarp3D")
	("g_tCubeMap",            "TextureCubeMap")
	("g_tEnvironmentMap",     "TextureEnvironmentMap");

void S2Decompiler::DecompileVMAT(const std::string& szFilename, const std::string& szOutputDirectory)
{
	char szBuffer[4];

	int32_t i, j;
	uint32_t nNumBlocks;

	KeyValues sRERLInfo, sNTROInfo;
	KeyValues sVMATData;

	std::fstream f;
	std::streamoff p1;
	f.open(szFilename, ios::in | ios::binary);
	if (!f.is_open())
		throw std::string("Could not open file \"" + szFilename + "\" for reading.");

	f.seekg(12);
	f.read((char *)&nNumBlocks, 4);
	for (nNumBlocks; nNumBlocks > 0; nNumBlocks--)
	{
		f.read(szBuffer, 4);
		if (strncmp(szBuffer, "RERL", 4) == 0)
		{
			ProcessRERLBlock(f, sRERLInfo);
		}
		else if (strncmp(szBuffer, "NTRO", 4) == 0)
		{
			ProcessNTROBlock(f, sNTROInfo);
		}
		else if (strncmp(szBuffer, "DATA", 4) == 0)
		{
			f.read(szBuffer, 4);
			p1 = f.tellg();
			f.seekg(*(int*)szBuffer - 4, ios::cur);
			ReadStructuredData(f, sVMATData, (KeyValues*)sNTROInfo.data[0]);
			f.seekg(p1 + 4);
		}
		else if (strncmp(szBuffer, "REDI", 4) == 0)
		{
			f.seekg(8, ios::cur);
		}
		else
		{
			throw std::string("Encountered invalid block type.");
		}
	}
	f.close();

	std::string szOutputName = szFilename.substr(szFilename.find_last_of("\\/") + 1);
	szOutputName = szOutputName.substr(0, szOutputName.find_last_of(".")) + ".vmat";

	f.open(szOutputDirectory + "\\" + szOutputName, ios::out);
	if (!f.is_open())
		throw std::string("Could not open file \"" + szOutputDirectory + "\\" + szOutputName + "\" for writing.");

	f << "// This file has been auto-generated by Source 2 Decompiler\n";
	f << "// https://github.com/Dingf/Source-2-Decompiler\n\n";
	f << "Layer0\n{ \n";

	const char* szShaderName = sVMATData["m_shaderName"];
	if (szShaderName == NULL)
		throw std::string("Could not find expected key value \"m_shaderName\".");

	f << "\tshader \"" << szShaderName << "\"\n\n";

	KeyValues* sIntParams = (KeyValues*)sVMATData["m_intParams"];
	if (sIntParams == NULL)
		throw std::string("Could not find expected key value \"m_intParams\".");
	for (i = 0; i < sIntParams->size; i++)
	{
		KeyValues* sIntParam = (KeyValues*)sIntParams->data[i];
		f << "\t" << sIntParam->data[0] << " " << *(int*)sIntParam->data[1] << "\n";
	}

	KeyValues* sIntAttribs = (KeyValues*)sVMATData["m_intAttributes"];
	if (sIntAttribs == NULL)
		throw std::string("Could not find expected key value \"m_intAttribs\".");
	for (i = 0; i < sIntAttribs->size; i++)
	{
		KeyValues* sIntAttrib = (KeyValues*)sIntAttribs->data[i];
		f << "\t" << sIntAttrib->data[0] << " " << *(int*)sIntAttrib->data[1] << "\n";
	}
	f << "\n";

	f.precision(6);

	KeyValues* sFloatParams = (KeyValues*)sVMATData["m_floatParams"];
	if (sFloatParams == NULL)
		throw std::string("Could not find expected key value \"m_floatParams\".");
	for (i = 0; i < sFloatParams->size; i++)
	{
		KeyValues* sFloatParam = (KeyValues*)sFloatParams->data[i];
		f << "\t" << sFloatParam->data[0] << " \"" << *(float*)sFloatParam->data[1] << "\"\n";
	}

	KeyValues* sFloatAttribs = (KeyValues*)sVMATData["m_floatAttributes"];
	if (sFloatAttribs == NULL)
		throw std::string("Could not find expected key value \"m_floatAttributes\".");
	for (i = 0; i < sFloatAttribs->size; i++)
	{
		KeyValues* sFloatAttrib = (KeyValues*)sFloatAttribs->data[i];
		f << "\t" << sFloatAttrib->data[0] << " \"" << *(float*)sFloatAttrib->data[1] << "\"\n";
	}
	f << "\n";

	KeyValues* sVectorParams = (KeyValues*)sVMATData["m_vectorParams"];
	if (sVectorParams == NULL)
		throw std::string("Could not find expected key value \"m_vectorParams\".");
	for (i = 0; i < sVectorParams->size; i++)
	{
		KeyValues* sVectorParam = (KeyValues*)sVectorParams->data[i];
		if (strncmp(sVectorParam->data[0], "g_vTexCoordScale", 16) == 0)
			f << "\t" << sVectorParam->data[0] << std::fixed << " \"[" << *(float*)&sVectorParam->data[1][0] << " " << *(float*)&sVectorParam->data[1][4] << "]\"\n\tg_vTexCoordOffset \"[" << *(float*)&sVectorParam->data[1][8] << " " << *(float*)&sVectorParam->data[1][12] << "]\"\n";
		else if (strncmp(sVectorParam->data[0], "g_vDetailTexCoordScale", 22) == 0)
			f << "\t" << sVectorParam->data[0] << std::fixed << " \"[" << *(float*)&sVectorParam->data[1][0] << " " << *(float*)&sVectorParam->data[1][4] << "]\"\n\tg_vDetailTexCoordOffset \"[" << *(float*)&sVectorParam->data[1][8] << " " << *(float*)&sVectorParam->data[1][12] << "]\"\n";
		else
			f << "\t" << sVectorParam->data[0] << std::fixed << " \"[" << *(float*)&sVectorParam->data[1][0] << " " << *(float*)&sVectorParam->data[1][4] << " " << *(float*)&sVectorParam->data[1][8] << " " << *(float*)&sVectorParam->data[1][12] << "]\"\n";
	}

	KeyValues* sVectorAttribs = (KeyValues*)sVMATData["m_vectorAttributes"];
	if (sVectorAttribs == NULL)
		throw std::string("Could not find expected key value \"m_vectorAttributes\".");
	for (i = 0; i < sVectorAttribs->size; i++)
	{
		KeyValues* sVectorAttrib = (KeyValues*)sVectorAttribs->data[i];
		f << "\t" << sVectorAttrib->data[0] << std::fixed << " \"[" << *(float*)&sVectorAttrib->data[1][0] << " " << *(float*)&sVectorAttrib->data[1][4] << " " << *(float*)&sVectorAttrib->data[1][8] << " " << *(float*)&sVectorAttrib->data[1][12] << "]\"\n";
	}
	f << "\n";

	KeyValues* sTextureParams = (KeyValues*)sVMATData["m_textureParams"];
	if (sTextureParams == NULL)
		throw std::string("Could not find expected key value \"m_textureParams\".");
	for (i = 0; i < sTextureParams->size; i++)
	{
		KeyValues* sTextureParam = (KeyValues*)sTextureParams->data[i];

		//Skip default resources; these will be added automatically by the Material Editor anyways
		if ((sTextureParam->data[1] == NULL) || (strncmp(sTextureParam->data[1], "materials/default/", 18) == 0))
			continue;

		DecompileVTEX(_szBaseDirectory + "\\" + sTextureParam->data[1] + "_c", szOutputDirectory, false, false);

		std::string szResourceString = sTextureParam->data[1];
		std::string szFileExt = szResourceString.substr(szResourceString.length() - 18, 5);
		std::string szImageName = sTextureParam->data[1];
		std::string szTexParamName = std::string(sTextureParam->data[0]);

		if ((szFileExt == "_tga_") || (szFileExt == "_psd_"))
			szImageName = szImageName.substr(0, szImageName.length() - 18);
		if (szResourceString.substr(szResourceString.length() - 23, 5) == "_z000")
		szImageName = szImageName.substr(0, szImageName.length() - 5);

		szImageName = szImageName.substr(szImageName.find_last_of("\\/") + 1);
		szImageName = szImageName.substr(0, szImageName.find_last_of(".")) + ".tga";
		if ((szTexParamName == "g_tColor1") || (szTexParamName == "g_tColor2") || (szTexParamName == "g_tColor3"))
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_a.tga";
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_ALPHA);
			f << "\tTextureColor" << szTexParamName[8] << " \"" << szImageName << "\"\n";
			f << "\tTextureRevealMask" << szTexParamName[8] << " \"" << szImageName2 << "\"\n";
		}
		else if (szTexParamName == "g_tColorTranslucency")
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_a.tga";
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_ALPHA);
			f << "\tTextureColor \"" << szImageName << "\"\n";
			f << "\tTextureTranslucency \"" << szImageName2 << "\"\n";
		}
		else if (szTexParamName == "g_tNormal")
		{
			FillImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName, IMAGE_CHANNEL_RED, 128);
			FillImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName, IMAGE_CHANNEL_BLUE, 255);
			f << "\tTextureNormal \"" << szImageName << "\"\n";
		}
		//Unconfirmed about the specular part
		else if (szTexParamName == "g_tNormalSpecularMask")
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_a.tga";
			
			SwapImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName, IMAGE_CHANNEL_ALPHA, IMAGE_CHANNEL_RED);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_ALPHA);
			
			f << "\tTextureNormal \"" << szImageName << "\"\n";
			f << "\tTextureSpecularMask \"" << szImageName2 << "\"\n";
		}
		else if (szTexParamName == "g_tMasks1")
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_a.tga";
			std::string szImageName3 = szImageName.substr(0, szImageName.find_last_of(".")) + "_r.tga";
			std::string szImageName4 = szImageName.substr(0, szImageName.find_last_of(".")) + "_g.tga";
			std::string szImageName5 = szImageName.substr(0, szImageName.find_last_of(".")) + "_b.tga";

			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_ALPHA);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName3, IMAGE_CHANNEL_RED);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName4, IMAGE_CHANNEL_GREEN);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName5, IMAGE_CHANNEL_BLUE);

			f << "\tTextureSelfIllumMask \"" << szImageName2 << "\"\n";
			f << "\tTextureDetailMask \"" << szImageName3 << "\"\n";
			f << "\tTextureDiffuseWarpMask \"" << szImageName4 << "\"\n";
			f << "\tTextureMetalnessMask \"" << szImageName5 << "\"\n";
		}
		else if (szTexParamName == "g_tMasks2")
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_a.tga";
			std::string szImageName3 = szImageName.substr(0, szImageName.find_last_of(".")) + "_r.tga";
			std::string szImageName4 = szImageName.substr(0, szImageName.find_last_of(".")) + "_g.tga";
			std::string szImageName5 = szImageName.substr(0, szImageName.find_last_of(".")) + "_b.tga";

			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_ALPHA);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName3, IMAGE_CHANNEL_RED);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName4, IMAGE_CHANNEL_GREEN);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName5, IMAGE_CHANNEL_BLUE);

			f << "\tTextureSpecularExponent \"" << szImageName2 << "\"\n";
			f << "\tTextureSpecularMask \"" << szImageName3 << "\"\n";
			f << "\tTextureRimMask \"" << szImageName4 << "\"\n";
			f << "\tTextureTintByBaseMask \"" << szImageName5 << "\"\n";
		}
		else if (szTexParamName == "g_tColorWarp3D")
		{
			std::string szImageName2;
			for (uint8_t j = 0; j < 4; j++)
			{
				std::string szNewImageName = szImageName.substr(0, szImageName.find_last_of("."));
				szNewImageName += "_z" + std::to_string((j / 100) % 10) + std::to_string((j / 10) % 10) + std::to_string(j % 10) + ".tga";
				if (j == 0)
					szImageName2 = szNewImageName;
				SwapImageChannel(szOutputDirectory + "\\" + szNewImageName, szOutputDirectory + "\\" + szNewImageName, IMAGE_CHANNEL_RED, IMAGE_CHANNEL_BLUE);
			}
			if (szImageName2.empty())
				throw std::string("Invalid color warp 3D image.");
			f << "\tTextureColorWarp3D \"" << szImageName2 << "\"\n";
		}
		else if (szTexParamName == "g_tFresnelWarp")
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_r.tga";
			std::string szImageName3 = szImageName.substr(0, szImageName.find_last_of(".")) + "_g.tga";
			std::string szImageName4 = szImageName.substr(0, szImageName.find_last_of(".")) + "_b.tga";

			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_RED);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName3, IMAGE_CHANNEL_GREEN);
			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName4, IMAGE_CHANNEL_BLUE);

			f << "\tTextureFresnelWarpRim \"" << szImageName2 << "\"\n";
			f << "\tTextureFresnelWarpColor \"" << szImageName3 << "\"\n";
			f << "\tTextureFresnelWarpSpec \"" << szImageName4 << "\"\n";
		}
		else if (szTexParamName == "g_tScrollSpeed")
		{
			std::string szImageName2 = szImageName.substr(0, szImageName.find_last_of(".")) + "_a.tga";
			std::string szImageName3 = szImageName.substr(0, szImageName.find_last_of(".")) + "_g.tga";

			ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName3, IMAGE_CHANNEL_GREEN);
			
			if ((*sIntParams)["F_TRANSLUCENT"] != NULL)
			{
				ExtractImageChannel(szOutputDirectory + "\\" + szImageName, szOutputDirectory + "\\" + szImageName2, IMAGE_CHANNEL_ALPHA);
				f << "\tTextureTranslucency \"" << szImageName2 << "\"\n";
			}
			f << "\tTextureScrollSpeed \"" << szImageName3 << "\"\n";
		}
		else if (szTexParamAliases.count(szTexParamName))
		{
			std::string szTexParamAlias = szTexParamAliases[szTexParamName];
			f << "\t" << szTexParamAlias << " \"" << szImageName << "\"\n";
		}
		else
		{
			std::cout << "\t" << szTexParamName << " " << szFilename << std::endl;
		}
	}
	f << "}";

	f.close();
}