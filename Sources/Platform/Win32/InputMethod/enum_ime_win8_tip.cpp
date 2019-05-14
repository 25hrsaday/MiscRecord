#include <windows.h>  
#include <msctf.h>  
#include <stdio.h>
#include <locale.h>   

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "CHS");
	CoInitialize(0);
	HRESULT hr = S_OK;

	//PunCha������Profiles�ӿڱ�  
	ITfInputProcessorProfiles *pProfiles;
	hr = CoCreateInstance(CLSID_TF_InputProcessorProfiles,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_ITfInputProcessorProfiles,
		(LPVOID*)&pProfiles);

	if (SUCCEEDED(hr))
	{
		IEnumTfLanguageProfiles* pEnumProf = 0;
		//PunCha��ö���������뷨��  
		hr = pProfiles->EnumLanguageProfiles(0x804, &pEnumProf);
		if (SUCCEEDED(hr) && pEnumProf)
		{
			TF_LANGUAGEPROFILE proArr[2];
			ULONG feOut = 0;
			//PunCha����ʵproArr����Ӧ��д�� &proArr[0]����Ϊ����ֻ��Ҫһ��TF_LANGUAGEPROFILE���������ң�proArr[1]��û�õ�����  
			while (S_OK == pEnumProf->Next(1, proArr, &feOut))
			{
				//PunCha����ȡ��������  
				BSTR bstrDest;
				hr = pProfiles->GetLanguageProfileDescription(proArr[0].clsid, 0x804, proArr[0].guidProfile, &bstrDest);
				// OutputDebugString(bstrDest);
				wprintf(bstrDest); 
				//MessageBox(0, bstrDest, 0, 0);
				printf("\n");

				BOOL bEnable = false;
				hr = pProfiles->IsEnabledLanguageProfile(proArr[0].clsid, 0x804, proArr[0].guidProfile, &bEnable);
				if (SUCCEEDED(hr))
				{
					printf("Enabled %d\n", bEnable);
				}
				SysFreeString(bstrDest);
			}
		}

		pProfiles->Release();
	}

	CoUninitialize();
	return 0;
}