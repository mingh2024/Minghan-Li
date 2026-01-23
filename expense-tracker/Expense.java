package main;

import java.time.LocalDate;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;

class Expense {
    private static final DateTimeFormatter DATE_FORMAT = DateTimeFormatter.ISO_LOCAL_DATE;
    private LocalDate date;
    private String description;
    private double amount;

    
    public Expense(LocalDate date, String description, double amount) {
        this.date = date;
        this.description = description;
        this.amount = amount;
    }
    

    public LocalDate getDate() {
        return date;
    }
    

    public String getDateString() {
        return date.format(DATE_FORMAT);
    }

    
    public String getDescription() {
        return description;
    }
    

    public double getAmount() {
        return amount;
    }

    
    public String toCsv() {
        return String.format("%s,%s,%.2f",
                escapeCsv(getDateString()),
                escapeCsv(description),
                amount);
    }

    
    public static Expense fromCsv(String line) {
        List<String> fields = parseCsvLine(line);
        if (fields.size() != 3) {
            throw new IllegalArgumentException("Invalid expense CSV line: " + line);
        }
        LocalDate date = LocalDate.parse(fields.get(0), DATE_FORMAT);
        String description = fields.get(1);
        double amount = Double.parseDouble(fields.get(2));
        return new Expense(date, description, amount);
    }

    
    private static String escapeCsv(String value) {
        String escaped = value.replace("\"", "\"\"");
        return "\"" + escaped + "\"";
    }

    
    private static List<String> parseCsvLine(String line) {
        List<String> fields = new ArrayList<>();
        StringBuilder current = new StringBuilder();
        boolean inQuotes = false;

        for (int i = 0; i < line.length(); i++) {
            char ch = line.charAt(i);
            if (ch == '"') {
                if (inQuotes && i + 1 < line.length() && line.charAt(i + 1) == '"') {
                    current.append('"');
                    i++;
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (ch == ',' && !inQuotes) {
                fields.add(current.toString());
                current.setLength(0);
            } else {
                current.append(ch);
            }
        }

        fields.add(current.toString());
        return fields;
    }
}
