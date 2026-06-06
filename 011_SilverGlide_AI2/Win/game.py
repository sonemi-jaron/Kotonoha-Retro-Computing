import pygame
import sys
import random

# --- 1. 初期設定と定数 ---
FPS = 60
SCREEN_WIDTH = 800
SCREEN_HEIGHT = 600

# カラー定義（サイバーネオン風）
WHITE = (255, 255, 255)
CYAN  = (0, 255, 255)
PINK  = (255, 0, 128)

# ゲーム状態の管理用定数
STATE_TITLE = 0
STATE_GAME = 1
STATE_GAMEOVER = 2

class SilverGlideModern:
    def __init__(self):
        pygame.init()
        self.screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
        pygame.display.set_caption("SILVER GLIDE 1 - Modern Remake")
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("arial", 36)
        self.large_font = pygame.font.SysFont("arial", 72)
        
        # --- 2. アセットの読み込みと最適化 ---
        try:
            # ニアレストネイバーのドット絵を読み込み
            self.player_img = pygame.image.load("player.png").convert_alpha()
            self.pillar_img = pygame.image.load("pillar.png").convert_alpha()
            self.bg_img = pygame.image.load("bg.jpg").convert()
            
            # スケーリング（ドットの質感を保つため標準のscaleを使用）
            self.player_img = pygame.transform.scale(self.player_img, (64, 64))
            self.pillar_img = pygame.transform.scale(self.pillar_img, (64, 250))
            self.bg_img = pygame.transform.scale(self.bg_img, (SCREEN_WIDTH, SCREEN_HEIGHT))
        except pygame.error as e:
            print(f"アセットの読み込みに失敗しました。画像が同じフォルダにあるか確認してください: {e}")
            sys.exit()

        # 起動時はタイトル画面からスタート
        self.state = STATE_TITLE
        self.bg_x = 0
        self.score = 0

    def reset_game(self):
        # --- 3. ゲーム状態の初期化 ---
        self.player_x = 100
        self.player_y = 300
        
        self.pillar_x = SCREEN_WIDTH
        self.pillar_height = random.choice([100, 170, 240])
        self.pillar_type = random.choice([0, 1])
        
        self.score = 0
        self.state = STATE_GAME

    def run(self):
        # メインループ
        while True:
            self.handle_events()
            self.update()
            self.draw()
            self.clock.tick(FPS)

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                # タイトル画面またはゲームオーバー画面で「1」キーが押されたらリスタート
                if event.key == pygame.K_1:
                    if self.state == STATE_TITLE or self.state == STATE_GAMEOVER:
                        self.reset_game()

    def update(self):
        # --- 5. 背景の無限スクロール（タイトル画面でもゆっくり流す） ---
        if self.state == STATE_GAME:
            self.bg_x -= 2
        else:
            self.bg_x -= 0.5 # タイトル・ゲームオーバー時は情緒的にゆっくり流す
            
        if self.bg_x <= -SCREEN_WIDTH:
            self.bg_x = 0

        # ゲーム中のみオブジェクトの移動や入力を処理
        if self.state == STATE_GAME:
            # --- 4. キー入力と自機制御 ---
            keys = pygame.key.get_pressed()
            if keys[pygame.K_1]:
                self.player_y -= 6  # 【1】キーで上昇
            else:
                self.player_y += 4  # 自由落下
                
            if self.player_y < 50:
                self.player_y = 50
            if self.player_y > SCREEN_HEIGHT - 64:
                self.state = STATE_GAMEOVER # 画面底タッチで死亡

            # --- 6. 柱の制御 ---
            self.pillar_x -= 5
            if self.pillar_x < -64:
                self.pillar_x = SCREEN_WIDTH
                self.pillar_height = random.choice([100, 170, 240])
                self.pillar_type = random.choice([0, 1])
                self.score += 1

            # --- 7. 当たり判定 ---
            player_rect = pygame.Rect(self.player_x, self.player_y, 48, 48)
            if self.pillar_type == 0:
                pillar_rect = pygame.Rect(self.pillar_x, 0, 64, self.pillar_height)
            else:
                pillar_rect = pygame.Rect(self.pillar_x, SCREEN_HEIGHT - self.pillar_height, 64, self.pillar_height)

            if player_rect.colliderect(pillar_rect):
                self.state = STATE_GAMEOVER

    def draw(self):
        # --- 8. 描画処理 ---
        # 背景（ループ描画）
        self.screen.blit(self.bg_img, (self.bg_x, 0))
        self.screen.blit(self.bg_img, (self.bg_x + SCREEN_WIDTH, 0))

        if self.state == STATE_GAME or self.state == STATE_GAMEOVER:
            # 柱の描画
            if self.pillar_type == 0:
                self.screen.blit(self.pillar_img, (self.pillar_x, 0), (0, 0, 64, self.pillar_height))
            else:
                flipped_pillar = pygame.transform.flip(self.pillar_img, False, True)
                self.screen.blit(flipped_pillar, (self.pillar_x, SCREEN_HEIGHT - self.pillar_height), (0, 250 - self.pillar_height, 64, self.pillar_height))

            # 自機の描画
            self.screen.blit(self.player_img, (self.player_x, self.player_y))

            # スコア表示
            score_text = self.font.render(f"SCORE: {self.score}", True, CYAN)
            self.screen.blit(score_text, (20, 15))

        # 【追加】タイトル画面のUI
        if self.state == STATE_TITLE:
            # 画面を少し暗くする
            overlay = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)
            overlay.fill((0, 0, 0, 100))
            self.screen.blit(overlay, (0, 0))
            
            title_text = self.large_font.render("SILVER GLIDE 1", True, PINK)
            subtitle_text = self.font.render("Modern Remake", True, WHITE)
            start_text = self.font.render("PRESS 1 KEY TO START", True, CYAN)
            
            self.screen.blit(title_text, (SCREEN_WIDTH//2 - title_text.get_width()//2, SCREEN_HEIGHT//2 - 100))
            self.screen.blit(subtitle_text, (SCREEN_WIDTH//2 - subtitle_text.get_width()//2, SCREEN_HEIGHT//2 - 20))
            self.screen.blit(start_text, (SCREEN_WIDTH//2 - start_text.get_width()//2, SCREEN_HEIGHT//2 + 80))

        # ゲームオーバー画面のUI
        if self.state == STATE_GAMEOVER:
            overlay = pygame.Surface((SCREEN_WIDTH, SCREEN_HEIGHT), pygame.SRCALPHA)
            overlay.fill((0, 0, 0, 180))
            self.screen.blit(overlay, (0, 0))
            
            go_text = self.large_font.render("GAME OVER", True, WHITE)
            final_score_text = self.font.render(f"FINAL SCORE: {self.score}", True, WHITE)
            restart_text = self.font.render("PRESS 1 TO RESTART", True, CYAN)
            
            self.screen.blit(go_text, (SCREEN_WIDTH//2 - go_text.get_width()//2, SCREEN_HEIGHT//2 - 80))
            self.screen.blit(final_score_text, (SCREEN_WIDTH//2 - final_score_text.get_width()//2, SCREEN_HEIGHT//2 - 10))
            self.screen.blit(restart_text, (SCREEN_WIDTH//2 - restart_text.get_width()//2, SCREEN_HEIGHT//2 + 60))

        pygame.display.flip()

if __name__ == "__main__":
    game = SilverGlideModern()
    game.run()