from pathlib import Path


PROJECT_ROOT = Path(__file__).resolve().parent.parent


def asset_path(*parts):
    return PROJECT_ROOT / 'assets' / Path(*parts)
