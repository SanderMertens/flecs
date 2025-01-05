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
    """Traverse directories, replace or move the first line depending on whether it's a comment."""
    comment_pattern = re.compile(r"^\s*//")

    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(('.h', '.cpp')):
                file_path = os.path.join(root, file)
                with open(file_path, 'r+', encoding='utf-8-sig') as f:
                    lines = f.readlines()
                    # Remove any leading ZWNBSP characters
                    lines = [line.replace("\ufeff", "") for line in lines]

                    if lines:
                        # Check if the first line is a comment
                        first_line = lines[0].rstrip("\n")  # Remove trailing newline without altering the content
                        if comment_pattern.match(first_line):
                            # Replace the first line with the new copyright notice
                            lines[0] = f"// {copyright_notice}\n"
                        else:
                            # Add the new copyright notice at the top and preserve the original first line
                            lines = [f"// {copyright_notice}\n\n"] + lines
                    else:
                        # If the file is empty, just add the copyright notice
                        lines.append(f"// {copyright_notice}\n")

                    # Write the modified content back to the file
                    f.seek(0)
                    f.writelines(lines)
                    f.truncate()
                    print(f"Processed copyright notice in: {file_path}")

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

    # Insert the copyright notice into source files in the target directory
    insert_copyright(target_directory, copyright_notice)

if __name__ == "__main__":
    main()
