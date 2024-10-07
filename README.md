[![CMake Build on multiple platforms](https://github.com/P0ulpy/log-compiler/actions/workflows/cmake-build-multi-platform.yml/badge.svg)](https://github.com/P0ulpy/log-compiler/actions/workflows/cmake-build-multi-platform.yml)

# Log Compiler

> Imagine a world with no limits and endless possibilities. In this marvelous world, it must have a Markdown compiler, right?... right? <br>
> Yes, I know what you're thinking: what kind of application could such a strange "compiler" have? <br>
> It doesn't have any real-world application, this is just a fun project I created to dive into the world of compiler development.<br>
> Don't take this project too seriously, it's intended for educational purposes. If the idea of a Markdown compiler offends you and you have a strong desire to tear your eyes out at the thought that Markdown can have some kind of runtime directly on your OS, it's normal. Stay calm and relax.

The Log Compiler is a tool designed to compile log files written in a specific Markdown-like format into structured data formats, such as `JSON` or Markdown. It can also be used to create `C` + `RayGui` code. So technically, your Markdown file can be compiled and have a runtime. How cool is that?

The project is currently in alpha stage of it's v1.0 (noted as v0.1), to see how the project is progressing see my [Features Checklist](TODO.md).

## Purpose

The primary purpose of this tool is to assist in processing and organizing log files that follow a specific syntax. It can be used to parse and transform these files into structured formats or generate `C` + `RayGui` code for custom visualizations and technically "compile" a Markdown file.

## Usage

To use the Log Compiler, provide an input log file in the specified format and specify the desired output format or other options using the CLI arguments. The tool will process the input file according to the grammar and generate the output as specified.

For more information on how to use the Log Compiler, refer to the project's documentation.

## CLI Arguments

The Log Compiler accepts the following command-line arguments:

**Usage :** `log-compiler InputFile [-o outputFile] [--format json|(markdown|md)|c] [--verbose] [--debug] [-v] [-h]`

**Positional arguments:**
- `InputFile`         : Specifies the input log file to be compiled.

**Optional arguments:**
- `-o`, `--output`    : Specifies the output file path (default=./a.{format}).
- `-f`, `--format`    : Specifies the output format as `JSON`, `Markdown` or `C + RayGui` format : `[json|(markdown|md)|c]` (default=json).
- `--verbose`         : Enables verbose mode for detailed output.
- `--debug`           : Enables debug mode for debugging information.
- `-v`, `--version`   : display compiler version, then exit.
- `-h`, `--help`      : display this message, then exit.

## Exemple Input File

```markdown
# Section 1
> This is a single line zone, and this text is its content.

This is a plain text
Wow what a text ! \
Wow what a second text !

# Section 2

This is a plain text

## Subsection
> This is a multiline zone.
> And this line and the above are its content.

This is a plain text
```

## Example Output files

**json :**
```json
{
    "name": "exemple",
    "content": [
        {
            "type": "section",
            "title": "Section 1",
            "level": 1,
            "content": [
                {
                    "type": "block-quotes",
                    "lines": [
                        "This is a single line zone, and this text is its content."
                    ]
                },
                {
                    "type": "plain-text",
                    "lines": [
                        "This is a plain text Wow what a text ! ",
                        "Wow what a second text !"
                    ]
                }
            ]
        },
        {
            "type": "section",
            "title": "Section 2",
            "level": 1,
            "content": [
                {
                    "type": "plain-text",
                    "lines": [
                        "This is a plain text"
                    ]
                },
                {
                    "type": "section",
                    "title": "Subsection",
                    "level": 2,
                    "content": [
                        {
                            "type": "block-quotes",
                            "lines": [
                                "This is a multiline zone.",
                                "And this line and the above are its content."
                            ]
                        },
                        {
                            "type": "plain-text",
                            "lines": [
                                "This is a plain text"
                            ]
                        }
                    ]
                }
            ]
        }
    ]
}
```

**C + RayGui (simplified) :**
```c
...(Custom RayGui declaration)

InitWindow(screenWidth, screenHeight, "exemple");

...

while (!WindowShouldClose())
{
    ClearBackground(RAYWHITE);

    BeginDrawing();

    GuiRenderTitle(1, "Section 1");
    
        GuiBeginBlock();
            GuiRenderText("This is a single line zone, and this text is its content.");
        GuiEndBlock();
    
        GuiRenderText("This is a plain text Wow what a text ! ");
        GuiRenderTextNoPaddingTop("Wow what a second text !");                        

    GuiRenderTitle(1, "Section 2");

        GuiRenderText("This is a plain text");

        GuiRenderTitle(2, "Subsection");

            GuiBeginBlock();
                GuiRenderText("This is a multiline zone.");
                GuiRenderText("And this line and the above are its content.");
            GuiEndBlock();

            GuiRenderText("This is a plain text");

    EndDrawing();
}

CloseWindow();

```

## Grammar (PEG Format)

Here's the grammar of the input file format recognized by the Log Compiler in PEG (Parsing Expression Grammar) format:

```peg
InputFile           <- Entry*
Entry               <- TitleSection / Blockquote / Line / EmptyLine
TitleSection        <- TitleLevel Text
TitleLevel          <- "#" ("#" / "#" / "#" / "#" / "#" / "#")?
Blockquote          <- BlockquoteLine+
BlockquoteLine      <- ">" Line BlockquoteLineEnd?
SpacingChar         <- "\"
Line                <- (!EndLine .)* SpacingChar? EndLine 
Text                <- (!EndLine .)+
EmptyLine           <- (" " / "\n" / "\r" / "\t")*
EndLine             <- "\n" / "\r" / "\r\n"
```

In this PEG-based grammar:

- `InputFile` consists of one or more `Entry` elements.
- `Entry` can be one of the following:
  - `TitleSection`: A section with a title.
  - `Blockquote`: A section with zones.
  - `Line`: A plain line of text.
- `EmptyLine`: A line containing only spaces, tabs, newlines, or carriage returns.
- `TitleSection`:
  - Begins with a `TitleLevel` that represents the section's level (up to six `#` symbols).
  - Followed by `Text` containing the section's title.
- `Blockquote`:
  - Consists of one or more `BlockquoteLine` elements.
- `BlockquoteLine`:
  - Starts with `>` and is followed by a `Line` of text.
- `Line`:
  - Represents plain lines of text without special markers.
  - A `\` character at the end character of a `Line` (before `EndLine`), represent a `SpacingChar`
- `Text`:
  - Represents any character sequence excluding newline characters.
- `EndLine`:
  - Represents the end of a line, which can be `\n`, `\r`, or `\r\n`.

For now, the grammar only contains `plain text`, `titles`, and `blockquotes` from Markdown, but I plan to add other Markdown syntax in the future like text format, links and more !

## License

This project is open-source and available under the [MIT License](LICENSE).
