# JSONFormatter

Example input:

```
{
	"test": "test : tests \\test \"test",
	[
		"simple",
		"as",
		"that"
	]
}
```

The source code lies in the `*.b` files. The entire program is split into the proper CGI script (that just pipes one tool into
another), URLEncode decoder and the formatter. Note that the behaviour might be undefined if the request is carefully forged.

As silly as it sounds, the project in the current state isn't a very good replacement for other JSON formatters out there, and
you shouldn't use it in production (unless you're fine with response travelling in up to 3 seconds in the example case).
