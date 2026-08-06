from dataclasses import dataclass
from typing import List

from models.audio import Audio


@dataclass(frozen=True)
class Song:
    id: str
    name: str
    autoforward: bool
    audios: List[Audio]
