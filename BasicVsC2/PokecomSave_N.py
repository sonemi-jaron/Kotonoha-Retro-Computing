# -*- coding: utf-8 -*-
import re
import os
import codecs

def export_for_pokecom():
    full_path = notepad.getCurrentFilename()
    file_dir, file_name = os.path.split(full_path)
    file_base, file_ext = os.path.splitext(file_name)
    
    ext_lower = file_ext.lower()
    is_c_lang = ext_lower in ['.c', '.cpp', '.h']
    is_asm = ext_lower in ['.asm', '.a']
    
    content = editor.getText()
    lines = content.splitlines()
    
    formatted_lines = []
    line_num = 10
    pending_label = "" # ラベルのみの行を一時保持する変数
    
    for line in lines:
        if is_asm:
            # 1. 注釈（; 以降）を削除
            clean_line = line.split(";")[0]
            # 2. 行末の空白削除（rstripでインデントは保持）
            clean_line = clean_line.rstrip()
            # 3. 空行スキップ
            if not clean_line.strip():
                continue
            
            # 4. ラベル行の判定（行頭が空白・タブ以外で始まり、コロンで終わる、または命令がない）
            # ラベル単独行の可能性があるかチェック
            is_label_only = False
            if clean_line and not clean_line.startswith((' ', '\t')):
                # ラベルらしきものがある場合、命令が続くか確認
                parts = re.split(r'\s+', clean_line, 1)
                if len(parts) == 1: # ラベル（またはシンボル）しかない
                    is_label_only = True
            
            if is_label_only:
                pending_label = clean_line
                continue # 次の行と結合するため、ここでは出力しない
            
            # 前の行にラベルがあった場合、現在の行（命令行）と結合
            if pending_label:
                clean_line = pending_label + clean_line.lstrip()
                pending_label = ""

        else:
            # BASIC / C言語用処理
            clean_line = line.strip()
            if not clean_line: continue
            if is_c_lang:
                clean_line = re.sub(r'/\*.*?\*/', '', clean_line)
                clean_line = re.sub(r'//.*', '', clean_line)
            else:
                clean_line = re.sub(r"'.*", '', clean_line)
                if clean_line.upper().startswith("REM"): continue
            clean_line = clean_line.strip()
            if not clean_line: continue

        # 行番号付与のロジック
        if is_asm and not clean_line.startswith((' ', '\t')):
            # ラベルで始まる行は、行番号とラベルの間のスペースを消す
            final_line = "{: >4}{}".format(line_num, clean_line)
        else:
            # 命令のみ（インデントあり）や他言語はスペースあり
            final_line = "{: >4} {}".format(line_num, clean_line)
            
        formatted_lines.append(final_line)
        line_num += 10

    # 最後にラベルだけ残ってしまった場合の処理
    if pending_label:
        final_line = "{: >4}{}".format(line_num, pending_label)
        formatted_lines.append(final_line)

    new_file_name = file_base + "_N" + file_ext
    new_full_path = os.path.join(file_dir, new_file_name)
    
    try:
        with codecs.open(new_full_path, 'w', 'ascii', 'ignore') as f:
            f.write('\r\n'.join(formatted_lines) + '\r\n')
        notepad.messageBox("保存完了: " + new_file_name, "Pokecom Save")
    except Exception as e:
        notepad.messageBox("エラー: " + str(e), "Pokecom Save Error")

export_for_pokecom()