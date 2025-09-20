import re
from pathlib import Path

def make_test_name(h2_title: str, h3_title: str, h4_title: str, test_number: int, file_path: str) -> str:
    """Construct a test name using available heading titles and test number.

    Falls back to the file stem if no headings are present.
    """
    parts = []

    def _clean(s: str) -> str:
        if not s:
            return ""
        clean = re.sub(r'[^a-zA-Z0-9_]', '_', s)
        clean = re.sub(r'_+', '_', clean).strip('_')
        return clean

    for t in (h2_title, h3_title, h4_title):
        c = _clean(t)
        if c:
            parts.append(c)

    parts.append(f"{test_number:02d}")

    if not parts or (len(parts) == 1 and parts[0].isdigit()):
        # fallback to file stem
        stem = Path(file_path).stem
        stem_clean = _clean(stem)
        parts = [stem_clean, f"{test_number:02d}"]

    return '_'.join(parts)
