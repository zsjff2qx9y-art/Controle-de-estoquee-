"""Testes pelo terminal: CRUD, persistencia polimorfica e protecao dos arquivos."""

import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest


EXECUTAVEL = Path(sys.argv.pop(1) if len(sys.argv) > 1 else "build/estoque").resolve()


class IntegracaoEstoque(unittest.TestCase):
    def setUp(self):
        self.temporario = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporario.cleanup)
        self.arquivo = Path(self.temporario.name) / "dados" / "estoque.txt"

    def executar(self, entrada, esperado=0):
        resultado = subprocess.run(
            [str(EXECUTAVEL), str(self.arquivo)],
            input=entrada,
            text=True,
            capture_output=True,
            timeout=5,
        )
        self.assertEqual(
            resultado.returncode, esperado, resultado.stdout + resultado.stderr
        )
        return resultado.stdout + resultado.stderr

    def gravar(self, conteudo):
        self.arquivo.parent.mkdir(parents=True, exist_ok=True)
        self.arquivo.write_text(conteudo, encoding="utf-8")

    def test_dois_tipos_persistem_e_relatorio_calcula_totais(self):
        self.executar(
            "1\n1\nLeite Integral\n4,25\n3\n2030-02-28\n"
            "1\n2\nCopo de Vidro\n10.00\n6\nUtensilios de cozinha\n7\n"
        )
        texto = self.arquivo.read_text(encoding="utf-8")
        self.assertIn('PERECIVEL "Leite Integral" 425 3', texto)
        self.assertIn('NAO_PERECIVEL "Copo de Vidro" 1000 6', texto)
        saida = self.executar("2\n6\n7\n")
        self.assertIn("Validade: 2030-02-28", saida)
        self.assertIn("Categoria: Utensilios de cozinha", saida)
        self.assertIn("Total: 2 cadastro(s), 9 unidade(s), R$ 72,75", saida)
        self.assertIn("Pereciveis: 1 cadastro(s), 3 unidade(s), R$ 12,75", saida)
        self.assertIn("Nao pereciveis: 1 cadastro(s), 6 unidade(s), R$ 60,00", saida)
        self.assertIn("estoque baixo (ate 5 unidades): 1", saida)

    def test_busca_nome_completo_prefixo_e_espacos(self):
        self.executar("1\n2\nCopo de Vidro\n2\n4\nCozinha\n7\n")
        saida = self.executar("3\nCopo de Vidro\n3\n  copo de  \n3\nAusente\n7\n")
        self.assertEqual(saida.count("Nome: Copo de Vidro"), 2)
        self.assertIn("Nenhum produto encontrado.", saida)

    def test_homonimos_exigem_indice_valido_da_busca(self):
        self.gravar(
            'ESTOQUE_V1\n'
            'PERECIVEL "Leite Integral" 100 1 "2026-01-01" "2030-01-01"\n'
            'NAO_PERECIVEL "Copo" 200 2 "2026-01-01" "Cozinha"\n'
            'PERECIVEL "Leite Integral" 300 3 "2026-01-01" "2031-01-01"\n'
        )
        saida = self.executar("3\nLeite Integral\n1\n2\n7\n")
        self.assertIn("Mais de um resultado.", saida)
        self.assertIn("Indice 0:", saida)
        self.assertIn("Indice 2:", saida)
        self.assertNotIn("Indice 1:", saida)
        self.assertIn("Escolha um dos indices exibidos.", saida)
        self.assertEqual(saida.count("Validade: 2031-01-01"), 2)

    def test_alteracao_e_remocao_atualizam_o_vetor_e_o_arquivo(self):
        self.gravar(
            'ESTOQUE_V1\n'
            'NAO_PERECIVEL "Caneca Azul" 500 4 "2025-03-12" "Cozinha"\n'
            'NAO_PERECIVEL "Caneca Azul" 600 2 "2025-03-13" "Cozinha"\n'
        )
        saida = self.executar(
            "4\nCaneca\n1\n1\nQueijo Minas\n12,50\n2\n2030-12-31\n"
            "5\nCaneca Azul\ns\n7\n"
        )
        self.assertIn("Produto alterado.", saida)
        self.assertIn("Produto removido.", saida)
        texto = self.arquivo.read_text(encoding="utf-8")
        self.assertNotIn("Caneca", texto)
        self.assertIn(
            'PERECIVEL "Queijo Minas" 1250 2 "2025-03-13" "2030-12-31"', texto
        )
        saida = self.executar("2\n6\n7\n")
        self.assertIn("Total: 1 cadastro(s), 2 unidade(s), R$ 25,00", saida)

    def test_cancelar_remocao_e_selecao_preserva_os_dados(self):
        self.executar(
            "1\n2\nCopo Azul\n1\n1\nCozinha\n"
            "1\n2\nCopo Verde\n2\n2\nCozinha\n7\n"
        )
        antes = self.arquivo.read_bytes()
        saida = self.executar("5\nCopo Azul\nn\n4\nCopo\n-1\n7\n")
        self.assertIn("Remocao cancelada.", saida)
        self.assertEqual(self.arquivo.read_bytes(), antes)

    def test_validacao_de_entrada_e_data_bissexta(self):
        saida = self.executar(
            "abc\n0\n1\n1\nLeite\n-1\n1,234\n0\n2,50\n"
            "-2\nnao\n3\n2023-02-29\n2024-02-29\n7\n"
        )
        self.assertIn("Digite um inteiro entre 1 e 7", saida)
        self.assertIn("Preco invalido.", saida)
        self.assertIn("Digite um inteiro entre 0 e 100000000", saida)
        texto = self.arquivo.read_text(encoding="utf-8")
        self.assertIn('PERECIVEL "Leite" 250 3', texto)
        self.assertIn('"2024-02-29"', texto)
        self.assertNotIn("2023-02-29", texto)

    def test_fim_da_entrada_salva_cadastro_completo_e_descarta_incompleto(self):
        saida = self.executar(
            "1\n2\nCaneca\n5,10\n2\nCozinha\n1\n1\nIncompleto\n"
        )
        self.assertIn("Fim da entrada.", saida)
        texto = self.arquivo.read_text(encoding="utf-8")
        self.assertIn('NAO_PERECIVEL "Caneca" 510 2', texto)
        self.assertNotIn("Incompleto", texto)

    def test_aspas_e_barras_sao_preservadas_na_serializacao(self):
        nome = 'Copo "Especial" \\ Azul'
        categoria = 'Linha "Premium" \\ Cozinha'
        self.executar(f"1\n2\n{nome}\n1,99\n2\n{categoria}\n7\n")
        saida = self.executar("2\n7\n")
        self.assertIn(f"Nome: {nome}", saida)
        self.assertIn(f"Categoria: {categoria}", saida)

    def test_arquivos_corrompidos_sao_rejeitados_sem_sobrescrever(self):
        casos = [
            "",
            "VERSAO_DESCONHECIDA\n",
            'ESTOQUE_V1\nOUTRO "Nome" 100 1 "2026-01-01" "Categoria"\n',
            'ESTOQUE_V1\nPERECIVEL "Leite" 100 -1 "2026-01-01" "2030-01-01"\n',
            'ESTOQUE_V1\nPERECIVEL "Leite" 100 1 "2026-01-01" "2030-02-30"\n',
            'ESTOQUE_V1\nNAO_PERECIVEL "Copo" 100 1 "2026-01-01" "Cozinha" extra\n',
        ]
        for conteudo in casos:
            with self.subTest(conteudo=conteudo):
                self.gravar(conteudo)
                antes = self.arquivo.read_bytes()
                saida = self.executar("7\n", esperado=1)
                self.assertIn("Erro:", saida)
                self.assertNotIn("Dados salvos.", saida)
                self.assertEqual(self.arquivo.read_bytes(), antes)

    @unittest.skipIf(os.name == "nt" or (hasattr(os, "geteuid") and os.geteuid() == 0),
                     "Permissoes POSIX requerem usuario sem privilegios de root")
    def test_falha_ao_salvar_retorna_erro_e_preserva_original(self):
        self.gravar("ESTOQUE_V1\n")
        antes = self.arquivo.read_bytes()
        self.arquivo.parent.chmod(0o555)
        try:
            saida = self.executar("7\n", esperado=1)
            self.assertNotIn("Dados salvos.", saida)
            self.assertEqual(self.arquivo.read_bytes(), antes)
        finally:
            self.arquivo.parent.chmod(0o755)


if __name__ == "__main__":
    unittest.main(verbosity=2)
