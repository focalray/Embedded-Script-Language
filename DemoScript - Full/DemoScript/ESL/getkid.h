

char* token_all[] = 
{
	";",
	"/",
	"/=",
	"+",
	"++",
	"+=",
	"-",
	"--",
	"*",
	"*=",
	"%",
	"%=",
	"(",
	")",
	"[",
	"]",
	".",
	"struct",
	"int",
	"int64",
	"short",
	"word",
	"dword",
	"byte",
	"float",
	"double",
	"string",
	"{",
	"}",
	"push",
	"pop",
	"peek",
	"popaway",
	"readstack",
	"loadmodule",
	"unloadmodule",
	"getret",
	"gethostret",
	"getproc", 
	"return", 
	"getportsize", 
	"getoffset", 
	"#portpack",
	"mapport",
	"putport",
	"readport",
	"echo",
	"echoln",
	"msg",
	"if",
	"else",
	"while",
	"break",
	"=",
	"goto",
	"@",
	"sin",
	"cos",
	"sqrt",
	"exp",
	"sleep",
	"call",
	"$",
	"&",
	"event",
	"callproc",
	"function",
	"end",
	"<",
	">",
};
	

unsigned char get_keywordid( char* keyword )
{
	for (int i=0; i<sizeof(token_all)/sizeof(token_all[0]); i++)
	{
		if ( TCMP( token_all[i], keyword ) )
			return i+1;
	}

	return 0;
}

char* get_keywordname( int id )
{
	return token_all[ id-1 ];
}
