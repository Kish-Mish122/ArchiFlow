# generate_icons.py - Icon generation utility
# This script generates required icons for the application

from PIL import Image, ImageDraw, ImageFont
import os

def create_icon(size, filename, color_scheme='blue'):
    """Create a simple icon for ArchiFlow"""
    
    # Create image with transparent background
    img = Image.new('RGBA', (size, size), (0, 0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # Color scheme
    if color_scheme == 'blue':
        primary = (0, 120, 215)
        secondary = (255, 255, 255)
        highlight = (0, 90, 158)
    elif color_scheme == 'dark':
        primary = (50, 50, 50)
        secondary = (200, 200, 200)
        highlight = (30, 30, 30)
    
    # Draw rounded rectangle
    padding = size // 10
    rect_coords = [padding, padding * 2, size - padding, size - padding // 2]
    
    # Main body
    draw.rounded_rectangle(rect_coords, radius=size//8, fill=primary)
    
    # Archive lines
    line_y = size // 3
    for i in range(3):
        y = line_y + i * (size // 8)
        draw.line([(size//4, y), (size*3//4, y)], 
                  fill=secondary, width=max(2, size//32))
    
    # Arrow/zipper
    arrow_center_x = size // 2
    arrow_y_start = size // 4
    arrow_y_end = size * 3 // 4
    
    # Draw arrow
    draw.polygon([
        (arrow_center_x, arrow_y_end - size//10),
        (arrow_center_x - size//8, arrow_y_end - size//5),
        (arrow_center_x - size//16, arrow_y_end - size//5),
        (arrow_center_x - size//16, arrow_y_end - size//3),
        (arrow_center_x + size//16, arrow_y_end - size//3),
        (arrow_center_x + size//16, arrow_y_end - size//5),
        (arrow_center_x + size//8, arrow_y_end - size//5)
    ], fill=secondary)
    
    # Save as ICO
    img.save(filename, format='ICO', sizes=[(size, size)])
    
    # Also save as PNG for reference
    png_filename = filename.replace('.ico', '.png')
    img.save(png_filename, format='PNG')

def generate_all_icons():
    """Generate all required icons"""
    
    icons_dir = 'resources'
    os.makedirs(icons_dir, exist_ok=True)
    
    # Main application icon (256x256)
    create_icon(256, os.path.join(icons_dir, 'archiflow.ico'), 'blue')
    
    # Small icon (16x16)
    create_icon(16, os.path.join(icons_dir, 'archiflow_small.ico'), 'blue')
    
    # Archive icon (48x48)
    create_icon(48, os.path.join(icons_dir, 'archive.ico'), 'blue')
    
    # Extract icon (48x48)
    create_icon(48, os.path.join(icons_dir, 'extract.ico'), 'blue')
    
    print("All icons generated successfully!")

def create_wizard_images():
    """Create wizard images for installer"""
    
    width, height = 164, 314  # Standard Inno Setup wizard image size
    small_width, small_height = 55, 55  # Small wizard image
    
    # Create main wizard image
    wizard_img = Image.new('RGB', (width, height), (240, 240, 240))
    draw = ImageDraw.Draw(wizard_img)
    
    # Draw gradient background
    for i in range(height):
        r = int(240 - (i / height) * 50)
        g = int(240 - (i / height) * 50)
        b = int(240 - (i / height) * 30)
        draw.line([(0, i), (width, i)], fill=(r, g, b))
    
    # Draw ArchiFlow logo
    logo_text = "A"
    font_size = 80
    try:
        font = ImageFont.truetype("arial.ttf", font_size)
    except:
        font = ImageFont.load_default()
    
    # Center the text
    text_bbox = draw.textbbox((0, 0), logo_text, font=font)
    text_width = text_bbox[2] - text_bbox[0]
    text_height = text_bbox[3] - text_bbox[1]
    text_x = (width - text_width) // 2
    text_y = (height - text_height) // 3
    
    draw.text((text_x, text_y), logo_text, fill=(0, 120, 215), font=font)
    
    # Draw name
    name_text = "ArchiFlow"
    font_size_small = 20
    try:
        font_small = ImageFont.truetype("arial.ttf", font_size_small)
    except:
        font_small = ImageFont.load_default()
    
    name_bbox = draw.textbbox((0, 0), name_text, font=font_small)
    name_width = name_bbox[2] - name_bbox[0]
    name_x = (width - name_width) // 2
    name_y = text_y + text_height + 20
    
    draw.text((name_x, name_y), name_text, fill=(50, 50, 50), font=font_small)
    
    wizard_img.save(os.path.join('resources', 'wizard_image.bmp'), format='BMP')
    
    # Create small wizard image
    small_img = Image.new('RGB', (small_width, small_height), (0, 120, 215))
    draw_small = ImageDraw.Draw(small_img)
    
    try:
        font_small_icon = ImageFont.truetype("arial.ttf", 30)
    except:
        font_small_icon = ImageFont.load_default()
    
    small_bbox = draw_small.textbbox((0, 0), "A", font=font_small_icon)
    small_text_width = small_bbox[2] - small_bbox[0]
    small_text_height = small_bbox[3] - small_bbox[1]
    small_text_x = (small_width - small_text_width) // 2
    small_text_y = (small_height - small_text_height) // 2
    
    draw_small.text((small_text_x, small_text_y), "A", 
                    fill=(255, 255, 255), font=font_small_icon)
    
    small_img.save(os.path.join('resources', 'wizard_small.bmp'), format='BMP')
    
    print("Wizard images created successfully!")

if __name__ == "__main__":
    generate_all_icons()
    create_wizard_images()