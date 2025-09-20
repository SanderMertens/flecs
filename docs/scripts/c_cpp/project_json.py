import json
from pathlib import Path
from typing import List, Dict

def load_project_json(project_file: Path) -> Dict:
    """Load project.json and return parsed data. Panic if missing or invalid."""
    if not project_file.exists():
        raise FileNotFoundError(f"Required project.json not found at {project_file}")
    with open(project_file, 'r') as f:
        return json.load(f)

def write_project_json(project_file: Path, data: Dict):
    with open(project_file, 'w') as f:
        json.dump(data, f, indent=4)

def update_testsuite_with_cases(project_file: Path, suite_id: str, testcases: List[str]):
    data = load_project_json(project_file)
    if 'test' not in data:
        data['test'] = {'testsuites': []}

    # find existing suite
    suite = None
    for s in data['test']['testsuites']:
        if s.get('id') == suite_id:
            suite = s
            break

    if suite is None:
        suite = {'id': suite_id, 'testcases': []}
        data['test']['testsuites'].append(suite)

    suite['testcases'] = testcases
    write_project_json(project_file, data)
