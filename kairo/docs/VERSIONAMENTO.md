# Versionamento do KAIRO no GitHub

Sugestão simples (ajuste como preferir):

- `main`: sempre estável; só entra o que foi compilado e testado.
- `dev`: trabalho do dia a dia. Faça pull request de `dev` para `main`.
- Uma branch por versão grande de manutenção, se precisar (ex.: `release/0.3`).
- **Tags** marcam versões: `v0.2.0` (a sua base), `v0.3.0` (esta). Ao criar uma tag `v*`, o workflow **KAIRO Build** compila sozinho.
- Guarde o commit/tag do Godot upstream em que você se baseou (4.8 beta) no README, para facilitar atualizações futuras.

Primeiro envio:
```bash
cd kairo-0.3.0-2d-gray
git init -b main && git add -A && git commit -m "KAIRO 0.3.0 (base: Godot 4.8 beta)"
git remote add origin <url-do-seu-repositorio>
git push -u origin main
git tag v0.3.0 && git push origin v0.3.0   # dispara o build
```
Tamanho: ~364 MB sem histórico; nenhum arquivo passa de 20 MB (o limite do GitHub é 100 MB por arquivo).

Os workflows originais do Godot em `.github/workflows` (android_builds, linux_builds etc.) são do projeto upstream, usam serviços/segredos que você não tem e podem falhar. Se não quiser vê-los, apague os arquivos ou desative em Actions.
