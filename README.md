# Log Compiler

> Imagine a world with no limits and endless possibilities. In this marvelous world, it must have a Markdown compiler, right?... right? <br>
> Yes, I know what you're thinking: what kind of application could such a strange "compiler" have? <br>
> It doesn't have any real-world application, this is just a fun project I created to dive into the world of compiler development.<br>
> Don't take this project too seriously, it's intended for educational purposes. If the idea of a Markdown compiler offends you and you have a strong desire to tear your eyes out at the thought that Markdown can have some kind of runtime directly on your os, it's normal stay calm and relax.

The Log Compiler is a tool designed to compile log files written in a specific Markdown-like format into structured data formats, such as `JSON` or Markdown. It can also be used to create `C` + `RayGui` code. So technically, your Markdown file can be compiled and have a runtime. How cool is that ?

## Purpose

The primary purpose of this tool is to assist in processing and organizing log files that follow a specific syntax. It can be used to parse and transform these files into structured formats or generate `C` + `RayGui` code for custom visualizations and technically "compile" a Markdown file.

## Usage

To use the Log Compiler, provide an input log file in the specified format and specify the desired output format or other options using the CLI arguments. The tool will process the input file according to the grammar and generate the output as specified.

For more information on how to use the Log Compiler, refer to the project's documentation.

## CLI Arguments

The Log Compiler accepts the following command-line arguments:

- `-i` or `--input`: Specifies the input log file to be compiled.
- `-o` or `--output`: Specifies the output file path (optional, default=./a.{extention}).
- `--format`: Specifies the output format (e.g., JSON or Markdown, C+RayGui) (optional, default=json).
- `--verbose`: Enables verbose mode for detailed output (optional).
- `--debug`: Enables debug mode for debugging information (optional).


## Example Input File

```markdown
# Section 1
> This is a single line zone, and this text is its content.

This is a plain text

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
    "name" : "exemple",
    "content" : [
        {
            "type" : "section",
            "title" : "Section 1",
            "content" : [
                {
                    "type" : "block-quotes",
                    "text" : "This is a single line zone, and this text is its content."
                },
                {
                    "type" : "plain-text",
                    "text" : "This is a plain text"
                }
            ]
        },
        {
            "type" : "section",
            "title" : "Section 2",
            "content" : [
                {
                    "type" : "plain-text",
                    "text" : "This is a plain text"
                },
                {
                    "type" : "section",
                    "title" : "Subsection",
                    "content" : [
                        {
                            "type" : "block-quotes",
                            "text" : "This is a multiline zone.\nAnd this line and the above are its content."
                        },
                        {
                            "type" : "plain-text",
                            "text" : "This is a plain text"
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
    
        GuiRenderText("This is a plain text");

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
InputFile       <- Entry*
Entry           <- TitleSection / Blockquote / Line
TitleSection    <- TitleLevel Text
TitleLevel      <- "#" ("#" / "#" / "#" / "#" / "#" / "#")?
Blockquote      <- BlockquoteLine+
BlockquoteLine  <- ">" Line
Line            <- (!EndLine .)* EndLine
Text            <- (!EndLine .)+
EndLine         <- "\n"
```

In this PEG-based grammar:

- `InputFile` consists of one or more `Entry` elements.
- `Entry` can be one of the following:
  - `TitleSection`: A section with a title.
  - `Blockquote`: A section with zones.
  - `Line`: A plain line of text.
- `TitleSection`:
  - Begins with a `TitleLevel` that represents the section's level (up to six `#` symbols).
  - Followed by `Text` containing the section's title.
- `Blockquote`:
  - Consists of one or more `BlockquoteLine` elements.
- `BlockquoteLine`:
  - Starts with `>` and is followed by a `Line` of text.
- `Line`:
  - Represents plain lines of text without special markers.
- `Text`:
  - Represents any character sequence excluding newline characters.

For now, the grammar only contains `plain text`, `titles`, and `blockquotes` from Markdown, but I plan to add other Markdown syntax in the future.

## License

This project is open-source and available under the [MIT License](LICENSE).