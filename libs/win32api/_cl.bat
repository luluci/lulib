
@rem 第1コマンドライン引数に指定したファイルをclでコンパイルする
cl -MT -EHsc /Zc:wchar_t %1 %2 -Id:\\lib\\boost_1_45_0 -Ie:\\work\\cxx_mylib -Ie:\\work\\git\\lulib -ID:\\lib\\openssl\\include -D_WIN32_WINNT=0x0501
