from __future__ import annotations

import html
import os
import shutil
import subprocess
import textwrap
import zipfile
from pathlib import Path
from xml.sax.saxutils import escape

from PIL import Image, ImageDraw, ImageFont


ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "Jumpa_Verse_SDL2_Documentation.docx"
BUILD = ROOT / "doc_build_assets"

ORANGE = "F47C20"
DARK = "3A2418"
LIGHT = "FFF3E8"
MID = "FFD2A8"
CODE_BG = "1F1A17"
CODE_FG = "FFF8F0"


def ensure_build_dir() -> None:
    if BUILD.exists():
        shutil.rmtree(BUILD)
    (BUILD / "media").mkdir(parents=True)


def run_dot(name: str, dot: str) -> Path:
    dot_path = BUILD / f"{name}.dot"
    png_path = BUILD / "media" / f"{name}.png"
    dot_path.write_text(dot, encoding="utf-8")
    subprocess.run(["dot", "-Tpng", str(dot_path), "-o", str(png_path)], check=True)
    return png_path


def make_banner() -> Path:
    path = BUILD / "media" / "orange_banner.png"
    img = Image.new("RGB", (1600, 420), (244, 124, 32))
    draw = ImageDraw.Draw(img)
    for y in range(420):
        r = 244
        g = max(90, 124 - y // 12)
        b = max(20, 32 - y // 20)
        draw.line([(0, y), (1600, y)], fill=(r, g, b))
    try:
        font_big = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 76)
        font_small = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans.ttf", 34)
    except OSError:
        font_big = ImageFont.load_default()
        font_small = ImageFont.load_default()
    draw.rounded_rectangle((60, 70, 1540, 350), radius=28, fill=(255, 245, 235), outline=(90, 45, 20), width=4)
    draw.text((105, 115), "Jumpa Verse", fill=(58, 36, 24), font=font_big)
    draw.text((110, 220), "SDL2 Game Code Documentation • UML • Libraries • Examples", fill=(105, 67, 42), font=font_small)
    draw.rectangle((80, 320, 1520, 330), fill=(244, 124, 32))
    img.save(path)
    return path


def make_tile_diagram() -> Path:
    path = BUILD / "media" / "ground_tile_diagram.png"
    img = Image.new("RGB", (1200, 560), (255, 243, 232))
    draw = ImageDraw.Draw(img)
    try:
        font = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans.ttf", 26)
        bold = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 30)
    except OSError:
        font = ImageFont.load_default()
        bold = ImageFont.load_default()
    draw.text((40, 35), "ground_Class builds one platform from many SDL_Rect tiles", fill=(58, 36, 24), font=bold)
    x0, y0 = 100, 150
    for col in range(8):
        draw.rectangle((x0 + col * 110, y0, x0 + col * 110 + 100, y0 + 70), fill=(208, 111, 48), outline=(58, 36, 24), width=3)
        draw.text((x0 + col * 110 + 24, y0 + 20), f"top {col}", fill=(255, 255, 255), font=font)
        for row in range(1, 4):
            color = (128, 82, 47) if row % 2 else (155, 98, 58)
            draw.rectangle((x0 + col * 110, y0 + row * 75, x0 + col * 110 + 100, y0 + row * 75 + 70), fill=color, outline=(58, 36, 24), width=2)
    draw.text((100, 475), "ground: vector<SDL_Rect> for top tiles", fill=(58, 36, 24), font=font)
    draw.text((610, 475), "groundUnder: vector<vector<SDL_Rect>> for lower rows", fill=(58, 36, 24), font=font)
    img.save(path)
    return path


def make_scene_diagram() -> Path:
    path = BUILD / "media" / "scene_camera_diagram.png"
    img = Image.new("RGB", (1200, 520), (222, 241, 255))
    draw = ImageDraw.Draw(img)
    try:
        font = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans.ttf", 25)
        bold = ImageFont.truetype("/usr/share/fonts/TTF/DejaVuSans-Bold.ttf", 30)
    except OSError:
        font = ImageFont.load_default()
        bold = ImageFont.load_default()
    draw.text((40, 28), "Camera illusion: objects move left while the SDL camera stays fixed", fill=(58, 36, 24), font=bold)
    draw.rectangle((60, 95, 1140, 430), outline=(244, 124, 32), width=6)
    draw.text((75, 105), "Window / viewport", fill=(58, 36, 24), font=font)
    for x, y, w, h in [(110, 310, 360, 80), (575, 250, 410, 140), (1030, 340, 330, 50)]:
        draw.rectangle((x, y, x + w, y + h), fill=(189, 104, 50), outline=(58, 36, 24), width=3)
    for x, y in [(250, 310), (735, 250), (1160, 340)]:
        draw.polygon([(x, y), (x + 50, y), (x + 25, y - 55)], fill=(92, 52, 40), outline=(40, 22, 18))
    draw.line((690, 170, 410, 170), fill=(244, 124, 32), width=7)
    draw.polygon([(410, 170), (445, 150), (445, 190)], fill=(244, 124, 32))
    draw.text((475, 135), "cameraOffset is negative, so x positions decrease", fill=(58, 36, 24), font=font)
    img.save(path)
    return path


class DocBuilder:
    def __init__(self) -> None:
        self.body: list[str] = []
        self.rels: list[tuple[str, str, str]] = []
        self.media: list[Path] = []
        self.rid = 1

    def add_rel(self, target: str, typ: str) -> str:
        rid = f"rId{self.rid}"
        self.rid += 1
        self.rels.append((rid, typ, target))
        return rid

    def p(self, text: str = "", style: str | None = None, bold: bool = False, color: str | None = None, size: int | None = None, align: str | None = None, shade: str | None = None) -> None:
        ppr = []
        if style:
            ppr.append(f'<w:pStyle w:val="{style}"/>')
        if align:
            ppr.append(f'<w:jc w:val="{align}"/>')
        if shade:
            ppr.append(f'<w:shd w:fill="{shade}"/>')
        rpr = []
        if bold:
            rpr.append("<w:b/>")
        if color:
            rpr.append(f'<w:color w:val="{color}"/>')
        if size:
            rpr.append(f'<w:sz w:val="{size}"/>')
        run = f"<w:r><w:rPr>{''.join(rpr)}</w:rPr><w:t xml:space=\"preserve\">{escape(text)}</w:t></w:r>"
        self.body.append(f"<w:p><w:pPr>{''.join(ppr)}</w:pPr>{run}</w:p>")

    def heading(self, text: str, level: int = 1) -> None:
        self.p(text, style=f"Heading{level}")

    def bullet(self, text: str) -> None:
        self.body.append(
            f'<w:p><w:pPr><w:numPr><w:ilvl w:val="0"/><w:numId w:val="1"/></w:numPr></w:pPr>'
            f'<w:r><w:t xml:space="preserve">{escape(text)}</w:t></w:r></w:p>'
        )

    def code(self, code: str) -> None:
        for line in code.strip("\n").splitlines():
            self.body.append(
                f'<w:p><w:pPr><w:shd w:fill="{CODE_BG}"/><w:spacing w:before="0" w:after="0"/></w:pPr>'
                f'<w:r><w:rPr><w:rFonts w:ascii="Consolas" w:hAnsi="Consolas"/><w:color w:val="{CODE_FG}"/><w:sz w:val="19"/></w:rPr>'
                f'<w:t xml:space="preserve">{escape(line)}</w:t></w:r></w:p>'
            )

    def image(self, path: Path, caption: str, width_in: float = 6.5) -> None:
        idx = len(self.media) + 1
        suffix = path.suffix.lower()
        media_name = f"image{idx}{suffix}"
        shutil.copy(path, BUILD / "media" / media_name)
        self.media.append(BUILD / "media" / media_name)
        rid = self.add_rel(f"media/{media_name}", "http://schemas.openxmlformats.org/officeDocument/2006/relationships/image")
        with Image.open(path) as img:
            w, h = img.size
        cx = int(width_in * 914400)
        cy = int(cx * h / w)
        self.body.append(f"""
<w:p><w:pPr><w:jc w:val="center"/></w:pPr><w:r><w:drawing>
<wp:inline distT="0" distB="0" distL="0" distR="0" xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing">
<wp:extent cx="{cx}" cy="{cy}"/><wp:docPr id="{idx}" name="{escape(caption)}"/>
<a:graphic xmlns:a="http://schemas.openxmlformats.org/drawingml/2006/main"><a:graphicData uri="http://schemas.openxmlformats.org/drawingml/2006/picture">
<pic:pic xmlns:pic="http://schemas.openxmlformats.org/drawingml/2006/picture"><pic:nvPicPr><pic:cNvPr id="{idx}" name="{escape(caption)}"/><pic:cNvPicPr/></pic:nvPicPr>
<pic:blipFill><a:blip r:embed="{rid}" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships"/><a:stretch><a:fillRect/></a:stretch></pic:blipFill>
<pic:spPr><a:xfrm><a:off x="0" y="0"/><a:ext cx="{cx}" cy="{cy}"/></a:xfrm><a:prstGeom prst="rect"><a:avLst/></a:prstGeom></pic:spPr></pic:pic>
</a:graphicData></a:graphic></wp:inline></w:drawing></w:r></w:p>
""")
        self.p(caption, align="center", color="7A3D12", size=20)

    def page_break(self) -> None:
        self.body.append('<w:p><w:r><w:br w:type="page"/></w:r></w:p>')

    def build(self) -> None:
        document_xml = f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:document xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main"
xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships">
<w:body>{''.join(self.body)}
<w:sectPr><w:pgSz w:w="12240" w:h="15840"/><w:pgMar w:top="900" w:right="900" w:bottom="900" w:left="900" w:header="720" w:footer="720" w:gutter="0"/></w:sectPr>
</w:body></w:document>"""
        styles_xml = f"""<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:styles xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
<w:style w:type="paragraph" w:default="1" w:styleId="Normal"><w:name w:val="Normal"/><w:rPr><w:rFonts w:ascii="Calibri" w:hAnsi="Calibri"/><w:sz w:val="22"/><w:color w:val="{DARK}"/></w:rPr><w:pPr><w:spacing w:after="160"/></w:pPr></w:style>
<w:style w:type="paragraph" w:styleId="Title"><w:name w:val="Title"/><w:rPr><w:b/><w:sz w:val="56"/><w:color w:val="{ORANGE}"/></w:rPr><w:pPr><w:jc w:val="center"/><w:spacing w:after="240"/></w:pPr></w:style>
<w:style w:type="paragraph" w:styleId="Heading1"><w:name w:val="heading 1"/><w:basedOn w:val="Normal"/><w:next w:val="Normal"/><w:rPr><w:b/><w:sz w:val="34"/><w:color w:val="{ORANGE}"/></w:rPr><w:pPr><w:spacing w:before="360" w:after="160"/><w:keepNext/></w:pPr></w:style>
<w:style w:type="paragraph" w:styleId="Heading2"><w:name w:val="heading 2"/><w:basedOn w:val="Normal"/><w:next w:val="Normal"/><w:rPr><w:b/><w:sz w:val="28"/><w:color w:val="A94B12"/></w:rPr><w:pPr><w:spacing w:before="240" w:after="120"/><w:keepNext/></w:pPr></w:style>
<w:style w:type="paragraph" w:styleId="Heading3"><w:name w:val="heading 3"/><w:basedOn w:val="Normal"/><w:next w:val="Normal"/><w:rPr><w:b/><w:sz w:val="24"/><w:color w:val="7A3D12"/></w:rPr><w:pPr><w:spacing w:before="180" w:after="80"/><w:keepNext/></w:pPr></w:style>
</w:styles>"""
        numbering_xml = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<w:numbering xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main">
<w:abstractNum w:abstractNumId="0"><w:lvl w:ilvl="0"><w:start w:val="1"/><w:numFmt w:val="bullet"/><w:lvlText w:val="•"/><w:lvlJc w:val="left"/><w:pPr><w:ind w:left="720" w:hanging="360"/></w:pPr></w:lvl></w:abstractNum>
<w:num w:numId="1"><w:abstractNumId w:val="0"/></w:num>
</w:numbering>"""
        rels_xml = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">
<Relationship Id="rId1" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/officeDocument" Target="word/document.xml"/>
</Relationships>"""
        doc_rels = ['<?xml version="1.0" encoding="UTF-8" standalone="yes"?><Relationships xmlns="http://schemas.openxmlformats.org/package/2006/relationships">',
                    '<Relationship Id="rIdStyles" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles" Target="styles.xml"/>',
                    '<Relationship Id="rIdNumbering" Type="http://schemas.openxmlformats.org/officeDocument/2006/relationships/numbering" Target="numbering.xml"/>']
        for rid, typ, target in self.rels:
            doc_rels.append(f'<Relationship Id="{rid}" Type="{typ}" Target="{target}"/>')
        doc_rels.append("</Relationships>")
        content_types = ['<?xml version="1.0" encoding="UTF-8" standalone="yes"?><Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">',
                         '<Default Extension="rels" ContentType="application/vnd.openxmlformats-package.relationships+xml"/>',
                         '<Default Extension="xml" ContentType="application/xml"/>',
                         '<Default Extension="png" ContentType="image/png"/>',
                         '<Default Extension="jpeg" ContentType="image/jpeg"/>',
                         '<Default Extension="jpg" ContentType="image/jpeg"/>',
                         '<Override PartName="/word/document.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.document.main+xml"/>',
                         '<Override PartName="/word/styles.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.styles+xml"/>',
                         '<Override PartName="/word/numbering.xml" ContentType="application/vnd.openxmlformats-officedocument.wordprocessingml.numbering+xml"/>',
                         '<Override PartName="/docProps/core.xml" ContentType="application/vnd.openxmlformats-package.core-properties+xml"/>',
                         '<Override PartName="/docProps/app.xml" ContentType="application/vnd.openxmlformats-officedocument.extended-properties+xml"/>',
                         '</Types>']
        core = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
<cp:coreProperties xmlns:cp="http://schemas.openxmlformats.org/package/2006/metadata/core-properties" xmlns:dc="http://purl.org/dc/elements/1.1/" xmlns:dcterms="http://purl.org/dc/terms/" xmlns:dcmitype="http://purl.org/dc/dcmitype/" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"><dc:title>Jumpa Verse SDL2 Documentation</dc:title><dc:creator>Codex</dc:creator><dc:description>Orange themed SDL2 code documentation with UML, figures, examples, and project explanation.</dc:description></cp:coreProperties>"""
        app = """<?xml version="1.0" encoding="UTF-8" standalone="yes"?><Properties xmlns="http://schemas.openxmlformats.org/officeDocument/2006/extended-properties"><Application>Codex OOXML Generator</Application></Properties>"""
        if OUT.exists():
            OUT.unlink()
        with zipfile.ZipFile(OUT, "w", zipfile.ZIP_DEFLATED) as z:
            z.writestr("[Content_Types].xml", "".join(content_types))
            z.writestr("_rels/.rels", rels_xml)
            z.writestr("word/document.xml", document_xml)
            z.writestr("word/styles.xml", styles_xml)
            z.writestr("word/numbering.xml", numbering_xml)
            z.writestr("word/_rels/document.xml.rels", "".join(doc_rels))
            z.writestr("docProps/core.xml", core)
            z.writestr("docProps/app.xml", app)
            for media in self.media:
                z.write(media, f"word/media/{media.name}")


def uml_dot() -> str:
    return f"""
digraph G {{
graph [rankdir=TB, bgcolor="transparent", pad=0.25, nodesep=0.6, ranksep=0.6];
node [shape=record, style="rounded,filled", fillcolor="#FFF3E8", color="#F47C20", penwidth=2, fontname="DejaVu Sans", fontsize=11];
edge [color="#7A3D12", penwidth=2, fontname="DejaVu Sans"];
hitBox [label="{{hitBox|+ int x\\l+ int y\\l+ int w\\l+ int h\\l|+ hitBox(x,y,w,h)\\l+ hitBox()\\l}}"];
ground [label="{{ground_Class|+ int divw\\l+ int trapCount\\l+ vector of SDL_Rect ground\\l+ 2D vector of SDL_Rect groundUnder\\l+ SDL_Texture* groundTexture\\l+ SDL_Texture* ugTexture\\l+ int perboxwidth=50\\l+ int perboxheight=50\\l|+ rebuild()\\l+ seterForElements(...)\\l+ render(renderer)\\l}}"];
trap [label="{{trapSpike|+ SDL_Point p1, p2, p3\\l|+ setterNew(x)\\l+ render(renderer,cameraOffset)\\l+ updatePosition(x,y)\\l}}"];
option [label="{{optionBoxes|+ SDL_Texture* textTexture\\l+ SDL_Texture* exitTexture\\l+ bool state\\l+ bool stateExit\\l+ SDL_Rect box\\l+ SDL_Rect boxExit\\l+ string exitText\\l|+ box_Initializer(...)\\l+ render(renderer)\\l+ stateHolder(...)\\l+ ~optionBoxes()\\l}}"];
color [label="{{mainMenuColor|+ static SDL_Color background\\l+ static SDL_Color buttonNormal\\l+ static SDL_Color buttonHover\\l+ static SDL_Color buttonActive\\l+ static SDL_Color text\\l|static menu palette\\l}}"];
funcs [shape=record, fillcolor="#FFE0BF", label="{{Project Functions|init() / close()\\lground_generator()\\lgroundRandomgenerator()\\lfreeMemory()\\lcameraMovmentObj()\\ltrapTriangle()\\lrenderTrap()\\ltrapFreeMemory()\\lmenuScreen()\\l}}"];
hitBox -> ground [label="inherits"];
hitBox -> trap [label="inherits"];
hitBox -> option [label="inherits"];
option -> color [style=dashed, label="uses colors"];
funcs -> ground [style=dashed, label="creates/updates"];
funcs -> trap [style=dashed, label="creates/updates"];
funcs -> option [style=dashed, label="menu"];
}}
"""


def flow_dot() -> str:
    return """
digraph G {
graph [rankdir=TB, bgcolor="transparent"];
node [shape=box, style="rounded,filled", fillcolor="#FFF3E8", color="#F47C20", penwidth=2, fontname="DejaVu Sans"];
edge [color="#7A3D12", penwidth=2, fontname="DejaVu Sans"];
start [label="main() starts"];
init [label="init(): SDL_Init, TTF_Init, IMG_Init, create window and renderer"];
load [label="Load textures and font"];
world [label="Create ground vector and first random platform"];
menu [label="menuScreen(): Play or Exit"];
loop [label="Game loop: poll events, calculate deltaTime, update cameraOffset"];
gen [label="Generate more ground and traps when needed"];
move [label="cameraMovmentObj() and renderTrap() move world left"];
draw [label="Clear renderer, render ground, render traps, present frame"];
fps [label="Delay to target 60 FPS and print FPS"];
close [label="close(): destroy renderer/window and quit SDL"];
start -> init -> load -> world -> menu -> loop -> gen -> move -> draw -> fps -> loop;
menu -> close [label="Exit"];
loop -> close [label="SDL_QUIT"];
}
"""


def pipeline_dot() -> str:
    return """
digraph G {
graph [rankdir=LR, bgcolor="transparent"];
node [shape=box, style="rounded,filled", fillcolor="#FFF3E8", color="#F47C20", penwidth=2, fontname="DejaVu Sans"];
edge [color="#7A3D12", penwidth=2, fontname="DejaVu Sans"];
window [label="SDL_Window\\nvisible game window"];
renderer [label="SDL_Renderer\\nGPU/accelerated drawing target"];
textures [label="SDL_Texture\\nimages and text stored for fast rendering"];
rects [label="SDL_Rect\\nposition and size: x, y, w, h"];
draw [label="SDL_RenderCopy / filledTrigonRGBA\\ndraw into back buffer"];
present [label="SDL_RenderPresent\\nshow final frame"];
window -> renderer -> textures -> draw -> present;
rects -> draw;
}
"""


def add_content(doc: DocBuilder, images: dict[str, Path]) -> None:
    doc.image(images["banner"], "Orange theme title banner", width_in=6.8)
    doc.p("Jumpa Verse SDL2 Game Documentation", style="Title")
    doc.p("This document explains the current C++ SDL2 game code from basic SDL concepts up to the project structure, object model, rendering pipeline, procedural ground generation, menu system, traps, camera movement, and cleanup. The examples are intentionally simple so a beginner can understand each part before reading the actual project files.", align="center")
    doc.p("Project: Jumpa-Verse | Main language: C++ | Graphics library: SDL2 | Theme: Orange", align="center", color="A94B12", bold=True)
    doc.page_break()

    doc.heading("1. Project Overview")
    doc.p("Jumpa Verse is an SDL2 side-scrolling platform-style game prototype. SDL2 does not provide a built-in 2D camera, so this project creates the camera illusion by changing object x positions over time. Platforms and traps move left across the fixed window while the player would appear to move through the world.")
    for item in [
        "main.cpp owns the main game loop, timing, texture loading, font loading, rendering order, and shutdown.",
        "object.h defines the object structures: hitBox, ground_Class, trapSpike, mainMenuColor, and optionBoxes.",
        "ground.cpp creates the first platform, generates later random platforms, and removes old ground.",
        "traps.cpp creates triangular spike traps using SDL2_gfx and frees old trap objects.",
        "camera.cpp moves ground and trap positions to simulate camera movement.",
        "menuScreen.cpp renders the menu buttons and handles Play/Exit mouse input.",
        "Error_Handlers.cpp contains SDL, SDL_image, and SDL_ttf initialization plus cleanup."
    ]:
        doc.bullet(item)

    doc.heading("2. File Structure")
    doc.code("""
Jumpa-Verse/
  main.cpp                  -> program entry point and game loop
  object.h                  -> game classes and object data structures
  functions.h               -> shared function declarations
  Error_Handlers.cpp        -> SDL initialization and shutdown
  ground.cpp                -> random ground generation and ground cleanup
  traps.cpp                 -> spike trap generation and trap cleanup
  camera.cpp                -> camera illusion by moving object positions
  menuScreen.cpp            -> Play and Exit menu
  Images_textures/          -> platform images
  debrosee-font/            -> TTF font used by menu text
  Tutorial_Image/           -> SDL coordinate and triangle reference images
""")

    doc.heading("3. UML Object Structure")
    doc.image(images["uml"], "Figure 1: UML class and function relationship diagram", width_in=6.8)
    doc.p("The UML diagram shows that hitBox is the base rectangle structure. Ground platforms, spike traps, and option boxes inherit the same x, y, w, and h fields. This is useful because every visible object needs a position and size.")

    doc.heading("3.1 Class Details", 2)
    doc.heading("hitBox", 3)
    doc.p("Purpose: stores the rectangle information common to game objects.")
    doc.code("""
class hitBox {
public:
    int x, y, w, h;
    hitBox(int x, int y, int w, int h);
    hitBox();
};
""")
    doc.heading("ground_Class", 3)
    doc.p("Purpose: represents one platform. The top is stored in ground, and the underground body is stored in groundUnder. It uses shared textures, meaning the class does not load or own the image files itself.")
    for item in [
        "divw: number of 50-pixel columns across the platform width.",
        "trapCount: number of traps currently created on that platform.",
        "ground: one-dimensional vector of SDL_Rect top tiles.",
        "groundUnder: two-dimensional vector of SDL_Rect lower tiles.",
        "rebuild(): recalculates every tile rectangle when x/y/w/h changes.",
        "seterForElements(): changes platform bounds and rebuilds geometry.",
        "render(): draws the top and underground tiles with SDL_RenderCopy."
    ]:
        doc.bullet(item)
    doc.image(images["tiles"], "Figure 2: How ground_Class breaks one platform into SDL_Rect tiles", width_in=6.6)

    doc.heading("trapSpike", 3)
    doc.p("Purpose: represents one triangular spike. It inherits the hitBox rectangle but draws itself using three SDL_Point values. SDL2_gfx functions filledTrigonRGBA and trigonRGBA draw the filled triangle and outline.")
    doc.code("""
trapSpike(int x, int y, int w, int h) : hitBox(x, y, w, h) {
    p1 = {x, y};
    p2 = {x + 50, y};
    p3 = {x + 25, y - 50};
}
""")

    doc.heading("optionBoxes", 3)
    doc.p("Purpose: stores and renders the Play and Exit menu buttons. It uses SDL_ttf to convert button text into textures. The destructor frees textTexture and exitTexture, which is important because SDL textures use memory outside ordinary C++ objects.")

    doc.heading("4. Runtime Flow")
    doc.image(images["flow"], "Figure 3: Program flow from initialization to game loop and cleanup", width_in=6.8)
    doc.p("The game starts by creating SDL systems, then loading textures and fonts. The menu runs before the world update. After Play is clicked, each frame calculates deltaTime, updates the camera offset, creates new platforms if needed, deletes old objects, renders the frame, and delays to keep the frame rate near 60 FPS.")

    doc.heading("5. SDL2 Basics")
    doc.p("SDL2 is a cross-platform C library used from C++ to create windows, read input, draw graphics, play audio, and manage timing. It is low-level compared with a full game engine: SDL gives you the building blocks, while your code decides the game structure.")
    doc.heading("5.1 Minimal SDL Program", 2)
    doc.code("""
#include <SDL2/SDL.h>

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow(
        "Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN
    );
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
""")
    doc.p("Important idea: the renderer draws into a hidden back buffer. SDL_RenderPresent swaps that finished frame onto the screen.")
    doc.image(images["pipeline"], "Figure 4: SDL rendering pipeline used by the project", width_in=6.8)

    doc.heading("5.2 SDL Coordinates and Rectangles", 2)
    doc.p("SDL uses a screen coordinate system where x increases to the right and y increases downward. The top-left corner is usually (0, 0). An SDL_Rect stores x, y, width, and height.")
    if images.get("coord"):
        doc.image(images["coord"], "Figure 5: SDL2 coordinate reference from the project tutorial images", width_in=4.5)
    doc.code("""
SDL_Rect box;
box.x = 100;  // left position
box.y = 200;  // top position
box.w = 80;   // width
box.h = 50;   // height

SDL_SetRenderDrawColor(renderer, 255, 128, 0, 255);
SDL_RenderFillRect(renderer, &box);
""")

    doc.heading("5.3 Events and Input", 2)
    doc.p("SDL_Event stores input and window messages. SDL_PollEvent reads all waiting events. This project uses SDL_QUIT to close the game and SDL_MOUSEBUTTONDOWN to detect menu clicks.")
    doc.code("""
SDL_Event event;
while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
        quit = true;
    }
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        int mx = event.button.x;
        int my = event.button.y;
        // compare mx/my with a button rectangle
    }
}
""")

    doc.heading("5.4 Timing and Delta Time", 2)
    doc.p("deltaTime is the number of seconds since the previous frame. Movement becomes smoother because objects move according to elapsed time instead of assuming every computer runs the same number of frames per second.")
    doc.code("""
Uint32 currentTime = SDL_GetTicks();
float deltaTime = (currentTime - lastTime) / 1000.0f;
lastTime = currentTime;

float speed = 2.0f;
cameraOffset -= speed * deltaTime;
""")

    doc.heading("6. SDL_image")
    doc.p("SDL_image loads image files that plain SDL cannot load easily. In this project it is used for JPEG platform textures.")
    doc.code("""
if (!(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
    std::cerr << IMG_GetError() << std::endl;
}

SDL_Texture* groundTex = IMG_LoadTexture(renderer, "Images_textures/ground.jpeg");
SDL_Texture* ugTex = IMG_LoadTexture(renderer, "Images_textures/underGround.jpeg");

SDL_RenderCopy(renderer, groundTex, nullptr, &destinationRect);
""")
    doc.p("Best practice: load each texture once, reuse it many times, and destroy it when the program ends. The current ground_Class stores texture pointers as shared references.")

    doc.heading("7. SDL_ttf")
    doc.p("SDL_ttf loads TrueType fonts and creates text surfaces. The surface is then converted into an SDL_Texture for fast rendering.")
    doc.code("""
TTF_Init();
TTF_Font* font = TTF_OpenFont("debrosee-font/Debrosee-ALPnL.ttf", 100);

SDL_Color orange = {255, 127, 0, 255};
SDL_Surface* surface = TTF_RenderText_Blended(font, "Play", orange);
SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, surface);
SDL_FreeSurface(surface);

SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
SDL_DestroyTexture(textTexture);
TTF_CloseFont(font);
TTF_Quit();
""")
    doc.p("In menuScreen.cpp, optionBoxes uses this same technique for the Play and Exit labels. Hovering changes state, then the text texture is recreated with a different color.")

    doc.heading("8. SDL2_gfx")
    doc.p("SDL2_gfx adds drawing primitives that SDL2 does not provide directly, such as filled triangles, circles, polygons, and anti-aliased lines. The project uses it for spike traps.")
    if images.get("tri"):
        doc.image(images["tri"], "Figure 6: Triangle color and point reference from the project tutorial images", width_in=6.3)
    doc.code("""
#include <SDL2/SDL2_gfxPrimitives.h>

filledTrigonRGBA(renderer,
    p1.x, p1.y,
    p2.x, p2.y,
    p3.x, p3.y,
    92, 52, 40, 255);

trigonRGBA(renderer,
    p1.x, p1.y,
    p2.x, p2.y,
    p3.x, p3.y,
    40, 22, 18, 255);
""")
    doc.p("The first call fills the triangle. The second call draws the outline. RGBA means red, green, blue, and alpha transparency.")

    doc.heading("9. Game Camera and World Movement")
    doc.image(images["scene"], "Figure 7: The camera illusion used by Jumpa Verse", width_in=6.8)
    doc.p("The project keeps the SDL window fixed. Instead of moving an actual camera, the code moves ground and trap objects by changing their x positions. When x gets smaller, the object appears to travel left.")
    doc.code("""
void cameraMovmentObj(std::vector<ground_Class>& ground, float cameraOffset,
                      std::vector<trapSpike>& trap) {
    for (int i = 0; i < ground.size(); i++) {
        ground[i].seterForElements(
            ground[i].x + (int)cameraOffset,
            ground[i].y,
            ground[i].w,
            ground[i].h
        );
    }
}
""")

    doc.heading("10. Procedural Ground and Traps")
    doc.p("ground_generator creates the first platform. groundRandomgenerator creates later platforms using random x, y, width, and height values based on the last platform. This makes the level continue without manually placing every platform.")
    doc.code("""
std::uniform_int_distribution<> distx(
    prevGround.back().x + prevGround.back().w + 120,
    prevGround.back().x + prevGround.back().w + 200
);

std::uniform_int_distribution<> disty(
    prevGround.back().y - 100,
    prevGround.back().y + 600
);
""")
    doc.p("trapTriangle may place a trap on a platform if a random chance succeeds and that platform has fewer than four traps. trapFreeMemory limits the trap vector so old traps do not grow forever.")

    doc.heading("11. Memory and Cleanup")
    for item in [
        "SDL_DestroyRenderer(renderer) frees the renderer.",
        "SDL_DestroyWindow(window) frees the window.",
        "SDL_DestroyTexture(texture) should be called for textures when no longer needed.",
        "SDL_FreeSurface(surface) is required after converting a text surface into a texture.",
        "IMG_Quit(), TTF_Quit(), and SDL_Quit() shut down SDL libraries.",
        "Vectors such as ground and trap should erase objects that are off-screen to avoid unnecessary memory use."
    ]:
        doc.bullet(item)
    doc.code("""
void close(SDL_Window* window, SDL_Renderer* renderer) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
}
""")
    doc.p("Recommended improvement for this project: close() should also call TTF_Quit(), and main.cpp should destroy groundTex and ugTex after the game loop.")

    doc.heading("12. Beginner-Friendly Build Notes")
    doc.p("A typical Linux compile command for this project should link SDL2, SDL2_image, SDL2_ttf, and SDL2_gfx.")
    doc.code("""
g++ main.cpp Error_Handlers.cpp ground.cpp camera.cpp traps.cpp menuScreen.cpp \\
    -o test \\
    -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_gfx
""")
    doc.p("If headers are missing, install the development packages for SDL2, SDL2_image, SDL2_ttf, and SDL2_gfx for your Linux distribution.")

    doc.heading("13. How To Read The Code")
    for item in [
        "Start from main.cpp because it shows the complete lifecycle.",
        "Read init() next to understand SDL setup.",
        "Read object.h to understand what data each object stores.",
        "Read ground.cpp and traps.cpp to understand level generation.",
        "Read camera.cpp to understand why objects move left.",
        "Read menuScreen.cpp to understand SDL mouse input and SDL_ttf text rendering."
    ]:
        doc.bullet(item)

    doc.heading("14. Suggested Improvements")
    for item in [
        "Add a player class with position, velocity, jump physics, collision checks, and animation state.",
        "Destroy loaded textures and close the font before exiting.",
        "Move constants such as window size, tile size, and colors into a config header.",
        "Use size_t for vector indexes where possible.",
        "Separate rendering, updating, and input into clearer functions as the game grows.",
        "Add collision detection between player hitBox and trapSpike/ground_Class."
    ]:
        doc.bullet(item)

    doc.heading("15. Quick Glossary")
    for item in [
        "Window: the operating-system window where the game appears.",
        "Renderer: SDL object used to draw images, rectangles, and primitives.",
        "Texture: GPU-friendly image used for fast rendering.",
        "Surface: CPU-side image data, often temporary before creating a texture.",
        "Rect: x/y/w/h rectangle used for position and size.",
        "Event: keyboard, mouse, quit, and other input message.",
        "Delta time: elapsed time between frames.",
        "RGBA: red, green, blue, alpha color values."
    ]:
        doc.bullet(item)


def main() -> None:
    ensure_build_dir()
    images = {
        "banner": make_banner(),
        "tiles": make_tile_diagram(),
        "scene": make_scene_diagram(),
        "uml": run_dot("uml_object_structure", uml_dot()),
        "flow": run_dot("runtime_flow", flow_dot()),
        "pipeline": run_dot("sdl_pipeline", pipeline_dot()),
    }
    coord = ROOT / "Tutorial_Image" / "Cordinate system sdl 2.png"
    tri = ROOT / "Tutorial_Image" / "TrigonRGB.png"
    if coord.exists():
        images["coord"] = coord
    if tri.exists():
        images["tri"] = tri
    doc = DocBuilder()
    add_content(doc, images)
    doc.build()
    print(OUT)


if __name__ == "__main__":
    main()
