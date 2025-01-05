import os
import json
import sys
import re

# Function to get copyright notice from .uplugin file
def get_copyright_from_uplugin(uplugin_path):
    with open(uplugin_path, 'r', encoding='utf-8') as f:
        data = json.load(f)
        return data.get("Copyright", "")

def insert_copyright(directory, copyright_notice):
    """Traverse directories, remove existing copyrights, and insert new copyright notice in .h and .cpp files."""
    copyright_pattern = re.compile(r"/\*.*?Copyright.*?\*/", re.DOTALL)

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.h', '.cpp')):
                file_path = os.path.join(root, file)
                with open(file_path, 'r+', encoding='utf-8') as f:
                    content = f.read()
                    # Remove all existing copyright notices
                    content = re.sub(copyright_pattern, "", content).strip()
                    # Add the new copyright notice at the top
                    updated_content = f"{copyright_notice}\n\n{content}"
                    f.seek(0)
                    f.write(updated_content)
                    f.truncate()
                    print(f"Updated copyright notice in: {file_path}")

def main():
    if len(sys.argv) != 3:
        print("Usage: python script_name.py <uplugin_file_path> <target_directory>")
        return

    # Get .uplugin file path and target directory from arguments
    uplugin_path = sys.argv[1]
    target_directory = sys.argv[2]

    # Validate the .uplugin file exists
    if not os.path.exists(uplugin_path):
        print("Error: .uplugin file not found at the specified path.")
        return

    # Extract the copyright notice from the .uplugin file
    copyright_notice = get_copyright_from_uplugin(uplugin_path)
    if not copyright_notice:
        print("Error: No copyright notice found in the .uplugin file.")
        return

    # Format the copyright notice as a comment block
    formatted_copyright = f"/*\n{copyright_notice}\n*/"

    # Insert the copyright notice into source files in the target directory
    insert_copyright(target_directory, formatted_copyright)

if __name__ == "__main__":
    main()
