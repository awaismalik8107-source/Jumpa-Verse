#!/usr/bin/env python3
import shutil
import tempfile
import zipfile
from pathlib import Path
import xml.etree.ElementTree as ET

ROOT = Path(__file__).resolve().parents[1]
DOCX = ROOT / "docs_report" / "JumpaVerse_SDL2_CPP_Game_Documentation.docx"

REL_NS = "http://schemas.openxmlformats.org/package/2006/relationships"
CT_NS = "http://schemas.openxmlformats.org/package/2006/content-types"
ET.register_namespace("", REL_NS)


def main():
    with tempfile.TemporaryDirectory() as td:
        work = Path(td)
        with zipfile.ZipFile(DOCX, "r") as zin:
            zin.extractall(work)

        rels_path = work / "word" / "_rels" / "document.xml.rels"
        tree = ET.parse(rels_path)
        root = tree.getroot()
        media = work / "word" / "media"
        media.mkdir(exist_ok=True)

        image_index = 1
        for rel in root.findall(f"{{{REL_NS}}}Relationship"):
            if rel.attrib.get("Type", "").endswith("/image") and rel.attrib.get("TargetMode") == "External":
                target = rel.attrib.get("Target", "")
                if target.startswith("file://"):
                    source = Path(target[7:])
                    if source.exists():
                        name = f"image{image_index}.png"
                        shutil.copy2(source, media / name)
                        rel.set("Target", f"media/{name}")
                        rel.attrib.pop("TargetMode", None)
                        image_index += 1

        tree.write(rels_path, encoding="UTF-8", xml_declaration=True)

        content_types = work / "[Content_Types].xml"
        ct_tree = ET.parse(content_types)
        ct_root = ct_tree.getroot()
        has_png = any(
            node.tag == f"{{{CT_NS}}}Default" and node.attrib.get("Extension") == "png"
            for node in ct_root
        )
        if not has_png:
            default = ET.Element(f"{{{CT_NS}}}Default", Extension="png", ContentType="image/png")
            ct_root.insert(0, default)
            ET.register_namespace("", CT_NS)
            ct_tree.write(content_types, encoding="UTF-8", xml_declaration=True)

        backup = DOCX.with_suffix(".linked-images.docx")
        if backup.exists():
            backup.unlink()
        DOCX.rename(backup)

        with zipfile.ZipFile(DOCX, "w", zipfile.ZIP_DEFLATED) as zout:
            for path in work.rglob("*"):
                if path.is_file():
                    zout.write(path, path.relative_to(work).as_posix())

    print(DOCX)


if __name__ == "__main__":
    main()
