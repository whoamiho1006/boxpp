#include <boxpp.hpp>
#include <Windows.h>
#pragma comment(lib, "boxpp.lib")
#pragma comment(lib, "boxpp-rt.lib")

boxpp::s32 run(boxpp::IBox* Box)
{
	boxpp::TSortedArray<boxpp::FName> Names;
	boxpp::TArray<boxpp::s32> Array;
	boxpp::TSortedArray<boxpp::s32,false> Array2;

	Array2.Add(130);
	Array2.Add(149);
	Array2.Add(145);
	Array2.Add(141);
	Array2.Add(146);
	Array2.Add(100);
	Array2.Add(170);

	Names.Add(L"Good1");
	Names.Add("Test2");
	Names.Add("Test1");
	Names.Add(L"Good2");
	Names.AddUnique("Test23");

	for (const boxpp::FName& Name : Names) {
		printf("%S\n", Name.GetRaw());
	}

	//Array.Sort(true);

	//Array.RemoveAt(0);

	Array2.RemoveLike([&](boxpp::s32 Item) {
		return Item < 140;
	});

	boxpp::s32 i = 0;
	Array = Array2;

	for (boxpp::s32 val : Array) {
		printf("%d, %d\n", i++, val);
	}

	printf("%d\n", Array.Search(141, true));

	boxpp::TStringConvert<boxpp::utf8_t, wchar_t> Test(L"한글을 변환");
	boxpp::TStringConvert<wchar_t, boxpp::utf8_t> Test2(Test.GetConvertedString());

	return 0;
}