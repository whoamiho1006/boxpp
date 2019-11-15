#include <boxpp.hpp>
#include <Windows.h>
#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

boxpp::s32 run(boxpp::IBox* Box)
{
	boxpp::TArray<boxpp::s32> Array;

	Array.Add(130);
	Array.Add(149);
	Array.Add(145);
	Array.Add(141);
	Array.Add(146);
	Array.Add(100);
	Array.Add(170);

	Array.Sort();

	boxpp::s32 i = 0;
	for (boxpp::s32 val : Array) {
		printf("%d, %d\n", i++, val);
	}

	printf("%d\n", Array.Search(141));

	boxpp::TStringConvert<boxpp::utf8_t, wchar_t> Test(L"한글을 변환");
	boxpp::TStringConvert<wchar_t, boxpp::utf8_t> Test2(Test.GetConvertedString());

	return 0;
}